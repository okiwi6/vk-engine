#ifndef window_hpp_
    #define window_hpp_

    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>

    #include <string>

    namespace vke {
        class VkeWindow {
            public:
            VkeWindow(int width, int height, std::string name);
            ~VkeWindow();

            VkeWindow(const VkeWindow&) = delete;
            VkeWindow &operator=(const VkeWindow&) = delete;

            bool should_close();

            void createWindowSurface(VkInstance instance, VkSurfaceKHR * surface);

            VkExtent2D getExtent();
            bool was_window_resized() { return frame_buffer_resized; }
            void reset_window_resized_flag() { frame_buffer_resized = false; }

            GLFWwindow* get_GLFW_window() const { return window; }

            private:
            static void frame_buffer_resize_callback(GLFWwindow *window, int width, int height);
            void initWindow();

            int width;
            int height;
            bool frame_buffer_resized = false;

            std::string window_name;
            GLFWwindow * window;
        };
        

    };

#endif