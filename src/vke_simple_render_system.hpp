#ifndef vke_simple_render_system_
    #define vke_simple_render_system_

    #include "vke_pipeline.hpp"
    #include "vke_device.hpp"
    #include "vke_game_object.hpp"
    #include "vke_camera.hpp"

    // std
    #include <memory>
    #include <vector>

    namespace vke {
        class VkeSimpleRenderSystem {
            public:
            VkeSimpleRenderSystem(VkeDevice &device, VkRenderPass render_pass);
            ~VkeSimpleRenderSystem();

            VkeSimpleRenderSystem(const VkeSimpleRenderSystem&) = delete;
            VkeSimpleRenderSystem& operator=(const VkeSimpleRenderSystem&) = delete;

            void render_game_objects(VkCommandBuffer command_buffer, std::vector<VkeGameObject> &game_objects, const VkeCamera &camera);
            
            private:
            void create_pipeline_layout();
            void create_pipeline(VkRenderPass render_pass);
            


            VkeDevice &vke_device;

            std::unique_ptr<VkePipeline> vke_pipeline;
            VkPipelineLayout pipeline_layout;
        };
    }

#endif