project (Vulkan1)

cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS};"-g")

include_directories(${PROJECT_SOURCE_DIR})

add_executable(vulkantest ./src/main.cpp ./src/window.cpp ./src/first_app.cpp ./src/vke_pipeline.cpp ./src/vke_device.cpp)

target_link_libraries(vulkantest -lglfw)
target_link_libraries(vulkantest -lvulkan)
target_link_libraries(vulkantest -ldl)
target_link_libraries(vulkantest -lpthread)
target_link_libraries(vulkantest -lX11)
target_link_libraries(vulkantest -lXxf86vm)
target_link_libraries(vulkantest -lXrandr)
target_link_libraries(vulkantest -lXi)