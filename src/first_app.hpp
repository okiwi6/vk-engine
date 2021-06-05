#ifndef first_app_
    #define first_app_

    #include "window.hpp"
    #include "vke_pipeline.hpp"

    namespace vke {
        class FirstApp {
            public:
            const int WIDTH = 800;
            const int HEIGHT = 600;

           void run();

            private:
            Window app_window{WIDTH, HEIGHT, "FirstApp", false};
            VkePipeline vke_pipeline{"../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv"};
        };
    }

#endif