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
                glm::vec3 position;
                glm::vec3 color;

                static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
                static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();

            };

            struct Data {
                std::vector<Vertex> vertices{};
                std::vector<uint32_t> indices{};
            };

            VkeModel(VkeDevice &device, const VkeModel::Data &model);
            ~VkeModel();
            VkeModel(const VkeModel&) = delete;
            VkeModel& operator=(const VkeModel&) = delete;

            void bind(VkCommandBuffer command_buffer);
            void draw(VkCommandBuffer command_buffer);

        private:
            void create_vertex_buffers(const std::vector<Vertex> &vertices);
            void create_index_buffers(const std::vector<uint32_t> &indices);

            VkeDevice& vke_device;
            
            VkBuffer vertex_buffer;
            VkDeviceMemory vertex_buffer_memory;
            uint32_t vertex_count;

            bool has_index_buffer{false};
            VkBuffer index_buffer;
            VkDeviceMemory index_buffer_memory;
            uint32_t index_count;
    };
}

#endif