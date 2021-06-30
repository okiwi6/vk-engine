#ifndef window_hpp_
    #define window_hpp_

    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>

    #include <string>

    namespace vke {
        class VkeWindow {
            public:
            VkeWindow(int width, int height, std::string name, bool full_screen);
            ~VkeWindow();

            VkeWindow(const VkeWindow&) = delete;
            VkeWindow &operator=(const VkeWindow&) = delete;

            bool should_close();

            void createWindowSurface(VkInstance instance, VkSurfaceKHR * surface);

            VkExtent2D getExtent();


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