#include "first_app.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include <array>

namespace vke {

    struct SimplePushConstantData {
        // identity
        glm::mat2 transform{1.0f};
        glm::vec2 offset;
        // alignment needed because 4 bytes expected
        alignas(16) glm::vec3 color;
    };


    FirstApp::FirstApp() {
        load_game_objects();
        createPipelineLayout();
        createPipeline();
    }

    FirstApp::~FirstApp() {
        vkDestroyPipelineLayout(vke_device.device(), pipeline_layout, nullptr);
        std::cout << "App closed successfully" << '\n';
    }


    void FirstApp::run() {
        while (!vke_window.should_close()) {;
            glfwPollEvents();

            if (VkCommandBuffer command_buffer = vke_renderer.begin_frame()) {
                vke_renderer.begin_swap_chain_render_pass(command_buffer);
                render_game_objects(command_buffer);
                vke_renderer.end_swap_chain_render_pass(command_buffer);
                vke_renderer.end_frame();
            }

        }

        vkDeviceWaitIdle(vke_device.device());
    }

    void FirstApp::load_game_objects() {
        // create triangle
        std::vector<VkeModel::Vertex> vertices {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };

        auto vke_model = std::make_shared<VkeModel>(vke_device, vertices);
        auto triangle = VkeGameObject::create_game_object();
        triangle.model = vke_model;
        triangle.color = {1.0f, 0.8f, 0.1f};
        triangle.transform2d.translation.x = 0.2f;
        triangle.transform2d.scale = {2.f, 1.f};
        triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

        game_objects.push_back(std::move(triangle));

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
        assert(pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipeline_config{};
        
        VkePipeline::defaultPipelineConfigInfo(pipeline_config);

        pipeline_config.render_pass = vke_renderer.get_swap_chain_render_pass();
        pipeline_config.pipeline_layout = pipeline_layout;
        vke_pipeline = std::make_unique<VkePipeline>(
            vke_device,
            "../shaders/simple_shader.vert.spv",
            "../shaders/simple_shader.frag.spv",
            pipeline_config
        );
    }

    void FirstApp::render_game_objects(VkCommandBuffer single_command_buffer) {
        vke_pipeline -> bind(single_command_buffer);

        for(auto& obj : game_objects) {
            obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());

            SimplePushConstantData push{};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;
            push.transform = obj.transform2d.mat2();

            vkCmdPushConstants(
                single_command_buffer,
                pipeline_layout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );
            obj.model->bind(single_command_buffer);
            obj.model->draw(single_command_buffer);
        }
    }
}