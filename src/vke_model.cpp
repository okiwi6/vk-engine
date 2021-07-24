#include "vke_model.hpp"

//std
#include <cassert>
#include <cstdint>
#include <cstring>
#include <vulkan/vulkan_core.h>

namespace vke {
    VkeModel::VkeModel(VkeDevice &device, const VkeModel::Data &data) :
        vke_device(device)
    {
        create_vertex_buffers(data.vertices);
        create_index_buffers(data.indices);
    }
            
    VkeModel::~VkeModel() {        
        vkDestroyBuffer(vke_device.device(), vertex_buffer, nullptr);
        vkFreeMemory(vke_device.device(), vertex_buffer_memory, nullptr);

        if(has_index_buffer) {
            vkDestroyBuffer(vke_device.device(), index_buffer, nullptr);
            vkFreeMemory(vke_device.device(), index_buffer_memory, nullptr);
        }
    }

    void VkeModel::create_vertex_buffers(const std::vector<Vertex> &vertices) {
        vertex_count = static_cast<uint32_t>(vertices.size());
        
        // there must exist at least 3 vertices
        assert(vertex_count >= 3 && "The vertex count is less than 3");

        VkDeviceSize buffer_size = sizeof(vertices[0]) * vertex_count;
        // host: cpu, device: gpu
        vke_device.createBuffer(
            buffer_size,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            vertex_buffer,
            vertex_buffer_memory
        );
        
        
        void *data;
        vkMapMemory(vke_device.device(), vertex_buffer_memory, 0, buffer_size, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
        
        // Flush not necessary because COHERENT flag set
        vkUnmapMemory(vke_device.device(), vertex_buffer_memory);
    }

    void VkeModel::create_index_buffers(const std::vector<uint32_t> &indices) {
        index_count = static_cast<uint32_t>(indices.size());
        has_index_buffer = index_count > 0;

        if(!has_index_buffer) {
            return;
        }

        VkDeviceSize buffer_size = sizeof(indices[0]) * index_count;
        // host: cpu, device: gpu
        vke_device.createBuffer(
            buffer_size,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            index_buffer,
            index_buffer_memory
        );
        
        
        void *data;
        vkMapMemory(vke_device.device(), index_buffer_memory, 0, buffer_size, 0, &data);
        memcpy(data, indices.data(), static_cast<size_t>(buffer_size));
        
        // Flush not necessary because COHERENT flag set
        vkUnmapMemory(vke_device.device(), index_buffer_memory);
    }

    void VkeModel::draw(VkCommandBuffer command_buffer) {
        if(has_index_buffer) {
            vkCmdDrawIndexed(command_buffer, index_count, 1, 0, 0, 0);
        } else {
            // 1 instance, 0 first vertex index, 0 first instance index
            vkCmdDraw(command_buffer, vertex_count, 1, 0, 0);
        }
    }

    void VkeModel::bind(VkCommandBuffer command_buffer) {
        VkBuffer buffers[] = {vertex_buffer};
        VkDeviceSize offsets[] = {0};
        // 0 first binding, 1 binding count
        vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);

        if(has_index_buffer) {
            vkCmdBindIndexBuffer(command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT32);
        }
    }

    std::vector<VkVertexInputBindingDescription> VkeModel::Vertex::get_binding_descriptions() {
        std::vector<VkVertexInputBindingDescription> binding_descriptions(1);
        binding_descriptions[0].binding = 0;
        binding_descriptions[0].stride = sizeof(Vertex);
        binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return binding_descriptions;
    }

    std::vector<VkVertexInputAttributeDescription> VkeModel::Vertex::get_attribute_descriptions() {
        std::vector<VkVertexInputAttributeDescription> attribute_descriptions(2);
        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[0].offset = offsetof(Vertex, position);

        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[1].offset = offsetof(Vertex, color);

        return attribute_descriptions;
    }


}
