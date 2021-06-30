#include "vke_window.hpp"
#include <stdexcept>

namespace vke {
        VkeWindow::VkeWindow(int w, int h, std::string name, bool full_screen=false):
            width(w),
            height(h),
            window_name(name),
            fullscreen(full_screen)
            {
                initWindow();
            }

        VkeWindow::~VkeWindow() {
            glfwDestroyWindow(window);
            glfwTerminate();
        }

        bool VkeWindow::should_close() {
            return glfwWindowShouldClose(window);
        }
        void VkeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR * surface) {
            if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
                throw std::runtime_error("failed to create window surface");
            }
        }

        void VkeWindow::initWindow() {
            glfwInit();

            // hint for vulkan
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            // disable resizing
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            // create window TODO
            window = glfwCreateWindow(width, height, window_name.c_str(), nullptr, nullptr);
        }

        VkExtent2D VkeWindow::getExtent() {
            return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
        }
}