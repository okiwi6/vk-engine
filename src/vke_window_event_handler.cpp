#include "vke_window_event_handler.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>

#include <unordered_map>

namespace vke {
    
    std::unordered_map<int, bool> VkeWindowKeyboardEventHandler::key_states = {};

    VkeWindowKeyboardEventHandler::VkeWindowKeyboardEventHandler()
    {}

    void VkeWindowKeyboardEventHandler::setup_callback(GLFWwindow* window) {
        std::cout << "Hello\n";
        glfwSetKeyCallback(window, this->key_pressed);
    }

    void VkeWindowKeyboardEventHandler::key_pressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if(key_states.find(scancode) == key_states.end()) {
            // key is not registered
            return;    
        }
        key_states[scancode] = (action == GLFW_PRESS);
        std::cout << scancode << "\n";
    }

    void VkeWindowKeyboardEventHandler::register_key(int key) {
        int scancode = glfwGetKeyScancode(key);
        std::cout << scancode << "\n";
        key_states[scancode] = 0;
    }

}