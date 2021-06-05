#ifndef window_hpp_
    #define window_hpp_

    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>

    #include <string>

    namespace vke {
        class Window {
            public:
            Window(int width, int height, std::string name, bool full_screen);
            ~Window();

            Window(const Window&) = delete;
            Window &operator=(const Window&) = delete;

            bool should_close();

            private:
            void initWindow();

            const int width;
            const int height;
            const bool fullscreen;

            std::string window_name;
            GLFWwindow * window;
        };
        

    };

#endif