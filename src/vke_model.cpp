#include "vke_model.hpp"
#include "vke_utils.hpp"

//libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

//std
#include <cassert>
#include <cstdint>
#include <cstring>
#include <vulkan/vulkan_core.h>
#include <unordered_map>

namespace std {
    template <>
    struct hash<vke::VkeModel::Vertex> {
        size_t operator()(vke::VkeModel::Vertex const &vertex) const {
            size_t seed = 0;
            vke::hash_combine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
}

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

    std::unique_ptr<VkeModel> VkeModel::create_model_from_file(VkeDevice &device, const std::string &filepath) {
        Data data{};
        data.load_model(filepath);

        // std::cout << "[i] Vertex Count " << data.vertices.size() << "\n";

        return std::make_unique<VkeModel>(device, data);
    }


    void VkeModel::create_vertex_buffers(const std::vector<Vertex> &vertices) {
        vertex_count = static_cast<uint32_t>(vertices.size());
        
        // there must exist at least 3 vertices
        assert(vertex_count >= 3 && "The vertex count is less than 3");

        VkDeviceSize buffer_size = sizeof(vertices[0]) * vertex_count;
        // host: cpu, device: gpu

        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        vke_device.createBuffer(
            buffer_size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            staging_buffer,
            staging_buffer_memory
        );
        
        
        void *data;
        vkMapMemory(vke_device.device(), staging_buffer_memory, 0, buffer_size, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
        
        // Flush not necessary because COHERENT flag set
        vkUnmapMemory(vke_device.device(), staging_buffer_memory);

        vke_device.createBuffer(
            buffer_size,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertex_buffer,
            vertex_buffer_memory
        );

        vke_device.copyBuffer(staging_buffer, vertex_buffer, buffer_size);

        vkDestroyBuffer(vke_device.device(), staging_buffer, nullptr);
        vkFreeMemory(vke_device.device(), staging_buffer_memory, nullptr);
    }

    void VkeModel::create_index_buffers(const std::vector<uint32_t> &indices) {
        index_count = static_cast<uint32_t>(indices.size());
        has_index_buffer = index_count > 0;

        if(!has_index_buffer) {
            return;
        }

        VkDeviceSize buffer_size = sizeof(indices[0]) * index_count;
        // host: cpu, device: gpu
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        vke_device.createBuffer(
            buffer_size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            staging_buffer,
            staging_buffer_memory
        );
        
        
        void *data;
        vkMapMemory(vke_device.device(), staging_buffer_memory, 0, buffer_size, 0, &data);
        memcpy(data, indices.data(), static_cast<size_t>(buffer_size));
        
        // Flush not necessary because COHERENT flag set
        vkUnmapMemory(vke_device.device(), staging_buffer_memory);

        vke_device.createBuffer(
            buffer_size,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            index_buffer,
            index_buffer_memory
        );

        vke_device.copyBuffer(staging_buffer, index_buffer, buffer_size);

        vkDestroyBuffer(vke_device.device(), staging_buffer, nullptr);
        vkFreeMemory(vke_device.device(), staging_buffer_memory, nullptr);
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


    void VkeModel::Data::load_model(const std::string &filepath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
            throw std::runtime_error(warn + err);
        }

        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex, uint32_t> unique_vertices{};

        for(const auto &shape : shapes) {
            for(const auto &index : shape.mesh.indices) {
                Vertex vertex{};

                if(index.vertex_index >= 0) {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0], 
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2],
                    };

                    // color support

                    auto color_index = 3 * index.vertex_index + 2;
                    if(color_index < attrib.colors.size()) {
                        vertex.color = {
                            attrib.colors[color_index - 2], 
                            attrib.colors[color_index - 1],
                            attrib.colors[color_index - 0],
                        };
                    } else {
                        // set default color
                        vertex.color = {1.f, 1.f, 1.f};
                    }
                }

                if(index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0], 
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                    };
                }

                if(index.texcoord_index >= 0) {
                    vertex.uv = {
                        attrib.texcoords[2 * index.texcoord_index + 0], 
                        attrib.texcoords[2 * index.texcoord_index + 1],
                    };
                }

                if(unique_vertices.count(vertex) == 0) {
                    // add to hashtable
                    unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(unique_vertices[vertex]);
            }
        }
    }

}
