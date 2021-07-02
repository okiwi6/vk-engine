
#ifndef vke_renderer_
    #define vke_renderer_

    #include "vke_window.hpp"
    #include "vke_device.hpp"
    #include "vke_swap_chain.hpp"

    // std
    #include <memory>
    #include <vector>
    #include <cassert>

    namespace vke {
        class VkeRenderer {
            public:
            VkeRenderer(VkeWindow& window, VkeDevice& device);
            ~VkeRenderer();

            VkeRenderer(const VkeRenderer&) = delete;
            VkeRenderer& operator=(const VkeRenderer&) = delete;

            VkRenderPass get_swap_chain_render_pass() const { return vke_swap_chain->getRenderPass(); }

            bool is_frame_in_progress() const { return is_frame_started; }

            VkCommandBuffer get_current_command_buffer() const { 
                assert(is_frame_started && "Cannot fetch command buffer if there is no frame in progress");
                return command_buffer[current_frame_index]; 
            }

            int get_frame_index() const {
                assert(is_frame_started && "Cannot get frame index when frame is not in progress");
                return current_frame_index;

            }

            VkCommandBuffer begin_frame();
            void end_frame();

            void begin_swap_chain_render_pass(VkCommandBuffer command_buffer);
            void end_swap_chain_render_pass(VkCommandBuffer command_buffer);

            private:
            void create_command_buffers();
            void free_command_buffers();
            void recreate_swap_chain();


            VkeWindow& vke_window;
            VkeDevice& vke_device;
            std::unique_ptr<VkeSwapChain> vke_swap_chain;
            std::vector<VkCommandBuffer> command_buffer;

            uint32_t current_image_index;
            int current_frame_index{0};
            bool is_frame_started{false};
        };
    }

#endif