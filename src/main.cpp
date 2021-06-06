#include "first_app.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

// cmake doesnt create MakeFile, can't compile

int main() {
    vke::FirstApp app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}