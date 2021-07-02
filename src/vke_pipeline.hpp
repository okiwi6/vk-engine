#ifndef vke_pipeline_
    #define vke_pipeline_

    #include <string>
    #include <vector>
#include <vulkan/vulkan_core.h>

    #include "vke_device.hpp"

    namespace vke {

        struct PipelineConfigInfo {
            // PipelineConfigInfo(const PipelineConfigInfo&) = delete;
            // PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;
            PipelineConfigInfo() = default;

            VkPipelineViewportStateCreateInfo viewport_info;
            VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
            VkPipelineRasterizationStateCreateInfo rasterization_info;
            VkPipelineMultisampleStateCreateInfo multisample_info;
            VkPipelineColorBlendAttachmentState color_blend_attachment;
            VkPipelineColorBlendStateCreateInfo color_blend_info;
            VkPipelineDepthStencilStateCreateInfo depth_stencil_info;

            std::vector<VkDynamicState> dynamics_state_enables;
            VkPipelineDynamicStateCreateInfo dynamics_state_info;

            VkPipelineLayout pipeline_layout = nullptr;
            VkRenderPass render_pass = nullptr;
            uint32_t subpass = 0;
        };

        class VkePipeline {
            // public
            public:
            VkePipeline(
                VkeDevice& device, 
                const std::string& vertex_shader_path, 
                const std::string& fragment_shader_path, 
                const PipelineConfigInfo& config_info
            );
            
            ~VkePipeline();

            VkePipeline(const VkePipeline&) = delete;
            VkePipeline& operator=(const VkePipeline&) = delete;

            static void defaultPipelineConfigInfo(PipelineConfigInfo& config_info);

            void bind(VkCommandBuffer command_buffer);

            // private
            private:
            static std::vector<char> readFile(const std::string& file_path);

            void createGraphicsPipeline(const std::string& vertex_shader_path, const std::string& fragment_shader_path, const PipelineConfigInfo& config_info);

            void createShaderModule(const std::vector<char>& code, VkShaderModule* shader_module);

            VkeDevice& vke_device;
            
            VkPipeline graphics_pipeline;
            VkShaderModule vert_shader_module;
            VkShaderModule fragment_shader_module;
        };
    }

#endif