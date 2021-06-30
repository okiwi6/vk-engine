#ifndef first_app_
    #define first_app_

    #include "window.hpp"
    #include "vke_pipeline.hpp"
    #include "vke_device.hpp"
    #include "vke_swap_chain.hpp"
    #include "vke_model.hpp"

    // std
    #include <memory>
    #include <vector>

    namespace vke {
        class FirstApp {
            public:
            const int WIDTH = 800;
            const int HEIGHT = 600;
            FirstApp();
            ~FirstApp();

            FirstApp(const FirstApp&) = delete;
            FirstApp& operator=(const FirstApp&) = delete;

            void run();

            private:
            void load_models();
            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();
            void drawFrame();

            Window vke_window;
            VkeDevice vke_device;
            VkeSwapChain vke_swap_chain;
            std::unique_ptr<VkePipeline> vke_pipeline;
            VkPipelineLayout pipeline_layout;
            std::vector<VkCommandBuffer> command_buffer;
            std::unique_ptr<VkeModel> vke_model;
        };
    }

#endif