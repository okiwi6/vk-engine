#include "vke_simple_render_system.hpp"


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
        glm::mat4 transform{1.0f};
        // alignment needed because 4 bytes expected
        alignas(16) glm::vec3 color;
    };


    VkeSimpleRenderSystem::VkeSimpleRenderSystem(VkeDevice &device, VkRenderPass render_pass) : vke_device(device)
    {
        create_pipeline_layout();
        create_pipeline(render_pass);
    }

    VkeSimpleRenderSystem::~VkeSimpleRenderSystem() {
        vkDestroyPipelineLayout(vke_device.device(), pipeline_layout, nullptr);
    }

    void VkeSimpleRenderSystem::create_pipeline_layout() {

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

    void VkeSimpleRenderSystem::create_pipeline(VkRenderPass render_pass) {
        assert(pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipeline_config{};
        
        VkePipeline::defaultPipelineConfigInfo(pipeline_config);

        pipeline_config.render_pass = render_pass;
        pipeline_config.pipeline_layout = pipeline_layout;
        vke_pipeline = std::make_unique<VkePipeline>(
            vke_device,
            "../shaders/simple_shader.vert.spv",
            "../shaders/simple_shader.frag.spv",
            pipeline_config
        );
    }

    void VkeSimpleRenderSystem::render_game_objects(VkCommandBuffer single_command_buffer, std::vector<VkeGameObject> &game_objects, const VkeCamera &camera) {
        vke_pipeline -> bind(single_command_buffer);

        auto projection_view = camera.get_projection() * camera.get_view();

        for(auto& obj : game_objects) {
            obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f, glm::two_pi<float>());
            obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.015f, glm::two_pi<float>());

            SimplePushConstantData push{};
            push.color = obj.color;
            push.transform = projection_view * obj.transform.mat4();

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