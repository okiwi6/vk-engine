#ifndef first_app_
    #define first_app_

    #include "window.hpp"
    #include "vke_pipeline.hpp"
    #include "vke_device.hpp"

    namespace vke {
        class FirstApp {
            public:
            const int WIDTH = 800;
            const int HEIGHT = 600;
            FirstApp();
            ~FirstApp();

            void run();

            private:
            Window vke_window;
            VkeDevice vke_device;
            VkePipeline vke_pipeline; 
        };
    }

#endif