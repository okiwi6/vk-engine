#include "first_app.hpp"

#include "vke_camera.hpp"
#include "vke_simple_render_system.hpp"
#include "keyboard_movement_controller.hpp"
#include "vke_definitions.hpp"

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
#include <chrono>

namespace vke {

    FirstApp::FirstApp() {
        load_game_objects();
    }

    FirstApp::~FirstApp() {
        std::cout << "App closed successfully" << '\n';
    }


    void FirstApp::run() {
        VkeSimpleRenderSystem simple_render_system{vke_device, vke_renderer.get_swap_chain_render_pass()};
        VkeCamera camera{};
        camera.set_view_direction(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.set_view_target(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewer_object = VkeGameObject::create_game_object();
        KeyboardMovementController camera_controller{};

        auto current_time = std::chrono::high_resolution_clock::now();


        while (!vke_window.should_close()) {
            glfwPollEvents();

            auto new_time = std::chrono::high_resolution_clock::now();
            float frame_time = std::chrono::duration<float, std::chrono::seconds::period>(new_time - current_time).count();
            current_time = new_time;

            frame_time = glm::min(frame_time, MAX_FRAME_TIME);

            camera_controller.move_in_plane_xz(vke_window.get_GLFW_window(), frame_time, viewer_object);
            camera.set_view_yxz(viewer_object.transform.translation, viewer_object.transform.rotation);


            float aspect = vke_renderer.get_aspect_ratio();
            // orthographic view
            // camera.set_orthographic_projection(-aspect, aspect, -1, 1, -1, 1);

            // perspective view
            camera.set_perspective_projection(glm::radians(50.f), aspect, 0.1f, 10.f);

            if (VkCommandBuffer command_buffer = vke_renderer.begin_frame()) {
                vke_renderer.begin_swap_chain_render_pass(command_buffer);
                simple_render_system.render_game_objects(command_buffer, game_objects, camera);
                vke_renderer.end_swap_chain_render_pass(command_buffer);
                vke_renderer.end_frame();
            }
            std::cout << "FPS: " << 1 / frame_time << std::endl;
        }

        vkDeviceWaitIdle(vke_device.device());
    }

std::unique_ptr<VkeModel> create_cube_model(VkeDevice& device, glm::vec3 offset) {
    VkeModel::Data model_data{};
    model_data.vertices = {
        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
    
        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
    
        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
    
        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
    
        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
    
        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    };

    for (auto& v : model_data.vertices) {
        v.position += offset;
    }

    // define index mapping
    model_data.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                          12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};


    return std::make_unique<VkeModel>(device, model_data);
}

    void FirstApp::load_game_objects() {
        std::shared_ptr<VkeModel> vke_model = create_cube_model(vke_device, {.0f, .0f, .0f});

        auto cube = VkeGameObject::create_game_object();
        cube.model = vke_model;
        cube.transform.translation = {.0f, .0f, 2.5f};
        cube.transform.scale = {0.5f, 0.5f, 0.5f};

        game_objects.push_back(std::move(cube));
    }
}