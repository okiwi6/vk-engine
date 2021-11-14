#include "first_app.hpp"

#include "vke_camera.hpp"
#include "vke_simple_render_system.hpp"
#include "keyboard_movement_controller.hpp"
#include "vke_definitions.hpp"
#include "vke_buffer.hpp"

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

    struct GlobalUBO {
        glm::mat4 projection_view{1.f};
        glm::vec4 ambient_light_color{1.f, 1.f, 1.f, .02f};
        glm::vec3 light_position{-1.f};
        alignas(16) glm::vec4 light_color{1.f}; //(r,g,b,intensity)
    };

    FirstApp::FirstApp() {
        global_pool = VkeDescriptorPool::Builder(vke_device)
            .set_max_sets(VkeSwapChain::MAX_FRAMES_IN_FLIGHT)
            .add_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VkeSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        load_game_objects();
    }

    FirstApp::~FirstApp() {
        std::cout << "App closed successfully" << '\n';
    }


    void FirstApp::run() {

        std::vector<std::unique_ptr<VkeBuffer>> ubo_buffers(VkeSwapChain::MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < ubo_buffers.size(); i++) {
            ubo_buffers[i] = std::make_unique<VkeBuffer>(
                vke_device,
                sizeof(GlobalUBO),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT /* | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT */
            );
            ubo_buffers[i]->map();

        }    

        
        auto global_set_layout = VkeDescriptorSetLayout::Builder(vke_device)
            .add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .build();

        
        std::vector<VkDescriptorSet> global_descriptor_sets(VkeSwapChain::MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < global_descriptor_sets.size(); i++) {
            auto buffer_info = ubo_buffers[i] -> descriptor_info();
            VkeDescriptorWriter(*global_set_layout, *global_pool)
                .write_buffer(0, &buffer_info)
                .build(global_descriptor_sets[i]);
        }

        VkeSimpleRenderSystem simple_render_system{
            vke_device, 
            vke_renderer.get_swap_chain_render_pass(), 
            global_set_layout->get_descriptor_set_layout()
        };

        VkeCamera camera{};
        camera.set_view_direction(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.set_view_target(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewer_object = VkeGameObject::create_game_object();
        viewer_object.transform.translation.z = -2.5f;
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
            camera.set_perspective_projection(glm::radians(50.f), aspect, 0.1f, 250.f);
            camera.set_perspective_projection(glm::radians(50.f), aspect, 0.1f, 250.f);

            if (VkCommandBuffer command_buffer = vke_renderer.begin_frame()) {
                int frame_index = vke_renderer.get_frame_index();
                FrameInfo frame_info{
                    frame_index,
                    frame_time,
                    command_buffer,
                    camera,
                    global_descriptor_sets[frame_index]
                };

                // update
                GlobalUBO ubo{};
                ubo.projection_view = camera.get_projection() * camera.get_view();
                ubo_buffers[frame_index]->write_to_buffer(&ubo);
                ubo_buffers[frame_index]->flush();
                
                // render
                vke_renderer.begin_swap_chain_render_pass(command_buffer);
                simple_render_system.render_game_objects(frame_info, game_objects);
                vke_renderer.end_swap_chain_render_pass(command_buffer);
                vke_renderer.end_frame();
            }
            // std::cout << "FPS: " << 1 / frame_time << std::endl;
        }

        vkDeviceWaitIdle(vke_device.device());
    }

    void FirstApp::load_game_objects() {
        std::shared_ptr<VkeModel> vke_model = VkeModel::create_model_from_file(vke_device, "../assets/flat_vase.obj");

        auto game_obj = VkeGameObject::create_game_object();
        game_obj.model = vke_model;
        game_obj.transform.translation = {-.5f, .5f, 0.f};
        game_obj.transform.scale = glm::vec3(3.f);

        game_objects.push_back(std::move(game_obj));

        vke_model = VkeModel::create_model_from_file(vke_device, "../assets/smooth_vase.obj");

        game_obj = VkeGameObject::create_game_object();
        game_obj.model = vke_model;
        game_obj.transform.translation = {.5f, .5f, 0.f};
        game_obj.transform.scale = glm::vec3(3.f);

        game_objects.push_back(std::move(game_obj));

        vke_model = VkeModel::create_model_from_file(vke_device, "../assets/quad.obj");

        auto game_obj2 = VkeGameObject::create_game_object();
        game_obj.model = vke_model;
        game_obj.transform.translation = {.0f, .5f, 0.f};
        game_obj.transform.scale = glm::vec3(3.f);

        game_objects.push_back(std::move(game_obj));
    }
}