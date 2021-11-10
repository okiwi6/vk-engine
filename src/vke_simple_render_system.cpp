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
        glm::mat4 model_matrix{1.0f};
        glm::mat4 normal_matrix{1.f};
    };


    VkeSimpleRenderSystem::VkeSimpleRenderSystem(VkeDevice &device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout) : 
        vke_device(device)
    {
        create_pipeline_layout(global_set_layout);
        create_pipeline(render_pass);
    }

    VkeSimpleRenderSystem::~VkeSimpleRenderSystem() {
        vkDestroyPipelineLayout(vke_device.device(), pipeline_layout, nullptr);
    }

    void VkeSimpleRenderSystem::create_pipeline_layout(VkDescriptorSetLayout global_set_layout) {

        // declare command buffer push data
        VkPushConstantRange push_constant_range{};
        push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_range.offset = 0;
        push_constant_range.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptor_set_layouts{global_set_layout};

        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
        pipeline_layout_info.pSetLayouts = descriptor_set_layouts.data();
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

    void VkeSimpleRenderSystem::render_game_objects(FrameInfo frame_info, std::vector<VkeGameObject> &game_objects) {
        vke_pipeline -> bind(frame_info.command_buffer);

        // rebind everything from beginning
        vkCmdBindDescriptorSets(
            frame_info.command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline_layout,
            0, 1,
            &frame_info.global_descriptor_set,
            0, 
            nullptr
        );

        for(auto& obj : game_objects) {
            SimplePushConstantData push{};
            
            push.model_matrix = obj.transform.mat4();
            push.normal_matrix = obj.transform.normal_matrix();

            vkCmdPushConstants(
                frame_info.command_buffer,
                pipeline_layout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );
            obj.model->bind(frame_info.command_buffer);
            obj.model->draw(frame_info.command_buffer);
        }
    }
}