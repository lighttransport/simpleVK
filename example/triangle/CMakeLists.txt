cmake_minimum_required(VERSION 2.8)

file(GLOB FILES *.cpp)

add_executable(Triangle ${FILES})
target_link_libraries(Triangle ${GLFW_STATIC_LIBRARIES} ${Vulkan_LIBRARIES})