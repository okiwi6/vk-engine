#include "window.hpp"

namespace vke {
        Window::Window(int w, int h, std::string name, bool full_screen=false):
            width(w),
            height(h),
            window_name(name),
            fullscreen(full_screen)
            {
                initWindow();
            }

        Window::~Window() {
            glfwDestroyWindow(window);
            glfwTerminate();
        }

        bool Window::should_close() {
            return glfwWindowShouldClose(window);
        }

        void Window::initWindow() {
            glfwInit();

            // hint for vulkan
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            // disable resizing
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            // create window TODO
            window = glfwCreateWindow(width, height, window_name.c_str(), nullptr, nullptr);
        }
}