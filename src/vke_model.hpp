#ifndef vke_model_
    #define vke_model_

#include "vke_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vke { 
    class VkeModel{
        public:

            struct Vertex {
                glm::vec2 position;

                static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
                static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();

            };

            VkeModel(VkeDevice &device, const std::vector<Vertex> &vertices);
            ~VkeModel();
            VkeModel(const VkeModel&) = delete;
            VkeModel& operator=(const VkeModel&) = delete;

            void bind(VkCommandBuffer command_buffer);
            void draw(VkCommandBuffer command_buffer);

        private:
            void create_vertex_buffers(const std::vector<Vertex> &vertices);

            VkeDevice& vke_device;
            VkBuffer vertex_buffer;
            VkDeviceMemory vertex_buffer_memory;
            uint32_t vertex_count;
    };
}

#endif