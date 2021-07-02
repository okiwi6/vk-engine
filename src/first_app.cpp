#include "first_app.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include <array>

namespace vke {

    struct SimplePushConstantData {
        glm::vec2 offset;
        // alignment needed because 4 bytes expected
        alignas(16) glm::vec3 color;
    };


    FirstApp::FirstApp() :
        vke_window(WIDTH, HEIGHT, "FirstApp", false),
        vke_device(vke_window)
    {
        load_models();
        createPipelineLayout();
        recreate_swap_chain();
        createCommandBuffers();
    }

    FirstApp::~FirstApp() {
        vkDestroyPipelineLayout(vke_device.device(), pipeline_layout, nullptr);
        std::cout << "App closed successfully" << '\n';
    }


    void FirstApp::run() {
        while (!vke_window.should_close()) {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(vke_device.device());
    }

    void FirstApp::load_models() {
        // create triangle
        std::vector<VkeModel::Vertex> vertices {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };

        vke_model = std::make_unique<VkeModel>(vke_device, vertices);
    }

    void FirstApp::createPipelineLayout() {

        // declare command buffer push data
        VkPushConstantRange push_constant_range{};
        push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_range.offset = 0;
        push_constant_range.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 0;
        pipeline_layout_info.pSetLayouts = nullptr;
        pipeline_layout_info.pushConstantRangeCount = 1;
        pipeline_layout_info.pPushConstantRanges = &push_constant_range;

        if ( vkCreatePipelineLayout(vke_device.device(), &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void FirstApp::createPipeline() {
        assert(vke_swap_chain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipeline_config{};
        
        VkePipeline::defaultPipelineConfigInfo(pipeline_config);

        pipeline_config.render_pass = vke_swap_chain -> getRenderPass();
        pipeline_config.pipeline_layout = pipeline_layout;
        vke_pipeline = std::make_unique<VkePipeline>(
            vke_device,
            "../shaders/simple_shader.vert.spv",
            "../shaders/simple_shader.frag.spv",
            pipeline_config
        );
    }

    void FirstApp::createCommandBuffers() {
        // one command buffer per frame buffer
        command_buffer.resize(vke_swap_chain -> imageCount());

        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = vke_device.getCommandPool();
        alloc_info.commandBufferCount = static_cast<uint32_t>(command_buffer.size());

        if (vkAllocateCommandBuffers(vke_device.device(), &alloc_info, command_buffer.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffer");
        }
  
    }

    void FirstApp::freeCommandBuffers() {
        vkFreeCommandBuffers(vke_device.device(), vke_device.getCommandPool(), static_cast<uint32_t>(command_buffer.size()), command_buffer.data());
        command_buffer.clear();
    }

    void FirstApp::recreate_swap_chain() {
        auto extent = vke_window.getExtent();
        while(extent.width == 0 || extent.height == 0) {
            extent = vke_window.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(vke_device.device());

        if (vke_swap_chain == nullptr) {
            vke_swap_chain = std::make_unique<VkeSwapChain>(vke_device, extent);
        } else {
            vke_swap_chain = std::make_unique<VkeSwapChain>(vke_device, extent, std::move(vke_swap_chain));
            if(vke_swap_chain -> imageCount() != command_buffer.size()) {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        // might not happen if renderpasses are compatible (optimizable)
        createPipeline();
    }

    void FirstApp::record_command_buffer(int image_index) {

        static int frame = 0;
        frame = (frame + 1) % 10;

        VkCommandBufferBeginInfo begin_info{};
            begin_info.sType  =VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if(vkBeginCommandBuffer(command_buffer[image_index], &begin_info) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer");
            }

            VkRenderPassBeginInfo render_pass_info{};
            render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_info.renderPass = vke_swap_chain -> getRenderPass();
            render_pass_info.framebuffer = vke_swap_chain -> getFrameBuffer(image_index);

            render_pass_info.renderArea.offset = {0, 0};
            render_pass_info.renderArea.extent = vke_swap_chain -> getSwapChainExtent();

            std::array<VkClearValue, 2> clear_values{};
            clear_values[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
            clear_values[1].depthStencil = {1.0f, 0};

            render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
            render_pass_info.pClearValues = clear_values.data();

            vkCmdBeginRenderPass(command_buffer[image_index], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
            
            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(vke_swap_chain->getSwapChainExtent().width);
            viewport.height = static_cast<float>(vke_swap_chain->getSwapChainExtent().height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            VkRect2D scissor{{0, 0}, vke_swap_chain->getSwapChainExtent()};
            vkCmdSetViewport(command_buffer[image_index], 0, 1, &viewport);
            vkCmdSetScissor(command_buffer[image_index], 0, 1, &scissor);


            vke_pipeline->bind(command_buffer[image_index]);
            vke_model -> bind(command_buffer[image_index]);

            for(int j = 0; j < 4; j++) {
                SimplePushConstantData push{};
                push.offset = {-0.5f + frame * 0.2f, -0.4f + j * 0.2f};
                push.color = {0.0f, 0.0f, 0.2f + 0.2f * j};

                vkCmdPushConstants(
                    command_buffer[image_index],
                    pipeline_layout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(SimplePushConstantData),
                    &push
                );
                vke_model -> draw(command_buffer[image_index]);
            }           


            vkCmdEndRenderPass(command_buffer[image_index]);
            if(vkEndCommandBuffer(command_buffer[image_index]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer");
            }
    }

    void FirstApp::drawFrame() {
        uint32_t image_index;
        auto result = vke_swap_chain -> acquireNextImage(&image_index);

        if(result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreate_swap_chain();
            return;
        }
        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to aquire swap chain image");
        }

        record_command_buffer(image_index);
        result = vke_swap_chain -> submitCommandBuffers(&command_buffer[image_index], &image_index);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vke_window.was_window_resized()) {
            vke_window.reset_window_resized_flag();
            recreate_swap_chain();
            return;
        }

        if(result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image");
        }
    }
}