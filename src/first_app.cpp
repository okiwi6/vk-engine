#include "first_app.hpp"
#include <iostream>

// std
#include <stdexcept>

namespace vke {
    FirstApp::FirstApp() :
        vke_window(WIDTH, HEIGHT, "FirstApp", false),
        vke_device(vke_window),
        vke_swap_chain(vke_device, vke_window.getExtent())
    {
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
        }
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

    void FirstApp::createCommandBuffers() {}

    void FirstApp::drawFrame() {}
}