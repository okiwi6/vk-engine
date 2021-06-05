#include "first_app.hpp"

namespace vke {
    void FirstApp::run() {
        while (!app_window.should_close()) {
            glfwPollEvents();
        }
    }
}