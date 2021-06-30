#include "first_app.hpp"
#include <iostream>

// std
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include <array>

namespace vke {
    FirstApp::FirstApp() :
        vke_window(WIDTH, HEIGHT, "FirstApp", false),
        vke_device(vke_window),
        vke_swap_chain(vke_device, vke_window.getExtent())
    {
        load_models();
        createPipelineLayout();
        createPipeline();
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
        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 0;
        pipeline_layout_info.pSetLayouts = nullptr;
        pipeline_layout_info.pushConstantRangeCount = 0;
        pipeline_layout_info.pPushConstantRanges = nullptr;
        if ( vkCreatePipelineLayout(vke_device.device(), &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void FirstApp::createPipeline() {
        auto pipeline_config = VkePipeline::defaultPipelineConfigInfo(vke_swap_chain.width(), vke_swap_chain.height());
        pipeline_config.render_pass = vke_swap_chain.getRenderPass();
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
        command_buffer.resize(vke_swap_chain.imageCount());

        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = vke_device.getCommandPool();
        alloc_info.commandBufferCount = static_cast<uint32_t>(command_buffer.size());

        if (vkAllocateCommandBuffers(vke_device.device(), &alloc_info, command_buffer.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffer");
        }

        for (int i = 0; i < command_buffer.size(); i++) {
            VkCommandBufferBeginInfo begin_info{};
            begin_info.sType  =VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if(vkBeginCommandBuffer(command_buffer[i], &begin_info) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer");
            }

            VkRenderPassBeginInfo render_pass_info{};
            render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_info.renderPass = vke_swap_chain.getRenderPass();
            render_pass_info.framebuffer = vke_swap_chain.getFrameBuffer(i);

            render_pass_info.renderArea.offset = {0, 0};
            render_pass_info.renderArea.extent = vke_swap_chain.getSwapChainExtent();

            std::array<VkClearValue, 2> clear_values{};
            clear_values[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
            clear_values[1].depthStencil = {1.0f, 0};

            render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
            render_pass_info.pClearValues = clear_values.data();

            vkCmdBeginRenderPass(command_buffer[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
            
            vke_pipeline->bind(command_buffer[i]);
            vke_model -> bind(command_buffer[i]);
            vke_model -> draw(command_buffer[i]);


            vkCmdEndRenderPass(command_buffer[i]);
            if(vkEndCommandBuffer(command_buffer[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer");
            }
        }   
    }

    void FirstApp::drawFrame() {
        uint32_t image_index;
        auto result = vke_swap_chain.acquireNextImage(&image_index);

        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to aquire swap chain image");
        }

        result = vke_swap_chain.submitCommandBuffers(&command_buffer[image_index], &image_index);

        if(result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image");
        }
    }
}