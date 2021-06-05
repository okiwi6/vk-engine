#ifndef vke_pipeline_
    #define vke_pipeline_

    #include <string>
    #include <vector>

    namespace vke {
        class VkePipeline {
            public:
            VkePipeline(const std::string& vertex_shader_path, const std::string& fragment_shader_path);

            private:
            static std::vector<char> readFile(const std::string& file_path);

            void createGraphicsPipeline(const std::string& vertex_shader_path, const std::string& fragment_shader_path);
        };
    }

#endif