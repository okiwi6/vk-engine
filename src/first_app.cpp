#include "first_app.hpp"
#include <iostream>

namespace vke {
    FirstApp::FirstApp() :
        vke_window(WIDTH, HEIGHT, "FirstApp", false),
        vke_device(vke_window),
        vke_pipeline(vke_device, "../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv", VkePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT))
    {}

    FirstApp::~FirstApp() {
        std::cout << "App closed successfully" << '\n';
    }


    void FirstApp::run() {
        while (!vke_window.should_close()) {
            glfwPollEvents();
        }
    }
}