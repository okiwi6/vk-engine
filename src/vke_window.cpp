#include "vke_window.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace vke {
        VkeWindow::VkeWindow(int w, int h, std::string name):
            width(w),
            height(h),
            window_name(name)
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
            // enable resizing
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

            // create window
            window = glfwCreateWindow(width, height, window_name.c_str(), nullptr, nullptr);
            glfwSetWindowUserPointer(window, this);
            
            // set callback function
            glfwSetFramebufferSizeCallback(window, frame_buffer_resize_callback);
        }

        VkExtent2D VkeWindow::getExtent() {
            return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
        }

        void VkeWindow::frame_buffer_resize_callback(GLFWwindow *window, int width, int height) {
            auto vke_window = reinterpret_cast<VkeWindow *>(glfwGetWindowUserPointer(window));
            vke_window -> frame_buffer_resized = true;
            vke_window -> width = width;
            vke_window -> height = height;
        }
}