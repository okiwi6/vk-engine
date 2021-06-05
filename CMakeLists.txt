project (Vulkan1)

cmake_minimum_required(VERSION 3.0)

add_executable(vulkantest ./src/main.cpp)

target_link_libraries(vulkantest -lglfw)
target_link_libraries(vulkantest -lvulkan)
target_link_libraries(vulkantest -ldl)
target_link_libraries(vulkantest -lpthread)
target_link_libraries(vulkantest -lX11)
target_link_libraries(vulkantest -lXxf86vm)
target_link_libraries(vulkantest -lXrandr)
target_link_libraries(vulkantest -lXi)