#ifndef first_app_
    #define first_app_

    #include "vke_window.hpp"
    #include "vke_device.hpp"
    #include "vke_game_object.hpp"
    #include "vke_renderer.hpp"

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
            void load_game_objects();;

            VkeWindow vke_window{WIDTH, HEIGHT, "vulkantest"};
            VkeDevice vke_device{vke_window};
            VkeRenderer vke_renderer{vke_window, vke_device};

            std::vector<VkeGameObject> game_objects;
        };
    }

#endif