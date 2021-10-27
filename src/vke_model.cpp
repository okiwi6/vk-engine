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
            
    VkeModel::~VkeModel() {}

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

        uint32_t vertex_size = sizeof(vertices[0]);
        VkeBuffer staging_buffer {
            vke_device,
            vertex_size,
            vertex_count,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,

        };

        staging_buffer.map();
        staging_buffer.write_to_buffer((void *) vertices.data());

        vertex_buffer = std::make_unique<VkeBuffer>(
            vke_device,
            vertex_size,
            vertex_count,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        vke_device.copyBuffer(staging_buffer.get_buffer(), vertex_buffer->get_buffer(), buffer_size);
    }

    void VkeModel::create_index_buffers(const std::vector<uint32_t> &indices) {
        index_count = static_cast<uint32_t>(indices.size());
        has_index_buffer = index_count > 0;

        if(!has_index_buffer) {
            return;
        }

        VkDeviceSize buffer_size = sizeof(indices[0]) * index_count;
        uint32_t index_size = sizeof(indices[0]);

        VkeBuffer staging_buffer {
            vke_device,
            index_size,
            index_count,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        staging_buffer.map();
        staging_buffer.write_to_buffer((void *) indices.data());
        
        index_buffer = std::make_unique<VkeBuffer>(
            vke_device,
            index_size,
            index_count,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        vke_device.copyBuffer(staging_buffer.get_buffer(), index_buffer->get_buffer(), buffer_size);
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
        VkBuffer buffers[] = {vertex_buffer->get_buffer()};
        VkDeviceSize offsets[] = {0};
        // 0 first binding, 1 binding count
        vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);

        if(has_index_buffer) {
            vkCmdBindIndexBuffer(command_buffer, index_buffer->get_buffer(), 0, VK_INDEX_TYPE_UINT32);
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
        std::vector<VkVertexInputAttributeDescription> attribute_descriptions{};

        // format (location, binding, format, offset)
        attribute_descriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
        attribute_descriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
        attribute_descriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
        attribute_descriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});


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
                    vertex.color = {
                        attrib.colors[3 * index.vertex_index + 0], 
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2],
                    };
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
