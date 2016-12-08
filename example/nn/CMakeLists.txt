cmake_minimum_required(VERSION 2.8)

file(GLOB FILES *.cpp)

add_executable(NeuralNetwork ${FILES})
target_link_libraries(NeuralNetwork ${GLFW_STATIC_LIBRARIES} ${Vulkan_LIBRARIES})
