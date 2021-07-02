#include "first_app.hpp"
#include "vke_simple_render_system.hpp"

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

    FirstApp::FirstApp() {
        load_game_objects();
    }

    FirstApp::~FirstApp() {
        std::cout << "App closed successfully" << '\n';
    }


    void FirstApp::run() {
        VkeSimpleRenderSystem simple_render_system{vke_device, vke_renderer.get_swap_chain_render_pass()};

        while (!vke_window.should_close()) {;
            glfwPollEvents();

            if (VkCommandBuffer command_buffer = vke_renderer.begin_frame()) {
                vke_renderer.begin_swap_chain_render_pass(command_buffer);
                simple_render_system.render_game_objects(command_buffer, game_objects);
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
}