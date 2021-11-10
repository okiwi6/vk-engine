#ifndef vke_frame_info_
    #define vke_frame_info_

#include "vke_camera.hpp"

// lib
#include <vulkan/vulkan.hpp>

namespace vke { 
    struct FrameInfo {
        int frame_index;
        float frame_time;
        VkCommandBuffer command_buffer;
        VkeCamera &camera;
        VkDescriptorSet global_descriptor_set;
    };
}

#endif