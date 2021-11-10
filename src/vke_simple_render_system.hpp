#ifndef vke_simple_render_system_
    #define vke_simple_render_system_

    #include "vke_pipeline.hpp"
    #include "vke_device.hpp"
    #include "vke_game_object.hpp"
    #include "vke_camera.hpp"
    #include "vke_frame_info.hpp"

    // std
    #include <memory>
    #include <vector>

    namespace vke {
        class VkeSimpleRenderSystem {
            public:
            VkeSimpleRenderSystem(VkeDevice &device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout);
            ~VkeSimpleRenderSystem();

            VkeSimpleRenderSystem(const VkeSimpleRenderSystem&) = delete;
            VkeSimpleRenderSystem& operator=(const VkeSimpleRenderSystem&) = delete;

            void render_game_objects(FrameInfo frame_info, std::vector<VkeGameObject> &game_objects);
            
            private:
            void create_pipeline_layout(VkDescriptorSetLayout global_set_layout);
            void create_pipeline(VkRenderPass render_pass);
            


            VkeDevice &vke_device;

            std::unique_ptr<VkePipeline> vke_pipeline;
            VkPipelineLayout pipeline_layout;
        };
    }

#endif