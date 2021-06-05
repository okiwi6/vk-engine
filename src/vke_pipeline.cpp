#include "vke_pipeline.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace vke {

    VkePipeline::VkePipeline(const std::string& vertex_shader_path, const std::string& fragment_shader_path) {
        createGraphicsPipeline(vertex_shader_path, fragment_shader_path);
    }

    std::vector<char> VkePipeline::readFile(const std::string& file_path) {
        std::ifstream file(file_path, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file " + file_path);
        }

        // last position (ate flag) is filesize
        size_t file_size = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(file_size);

        file.seekg(0);
        file.read(buffer.data(), file_size);

        file.close();
        return buffer;
    }

    void VkePipeline::createGraphicsPipeline(
        const std::string& vertex_shader_path, 
        const std::string& fragment_shader_path) {

        auto vert_code = readFile(vertex_shader_path);
        auto frag_code = readFile(fragment_shader_path);

        std::cout << "Vertex Shader Size: " << vert_code.size() << '\n';
        std::cout << "Fragment Shader Size: " << frag_code.size() << '\n';
    }
}