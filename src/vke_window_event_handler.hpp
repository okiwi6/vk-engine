#ifndef vke_window_event_handler_
    #define vke_window_event_handler_

#include <unordered_map>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vke {

class VkeWindowKeyboardEventHandler {
    public:
    VkeWindowKeyboardEventHandler();
    static void key_pressed(GLFWwindow* window, int key, int scancode, int action, int mods);
    void register_key(int key);
    void setup_callback(GLFWwindow* window);

    private:
    static std::unordered_map<int, bool> key_states;

};

}


#endif