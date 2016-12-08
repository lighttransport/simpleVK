cmake_minimum_required(VERSION 2.8)

project(SimpleVK)
if(WIN32)
find_package(GLFW REQUIRED)
else()
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
endif()
find_package(Vulkan REQUIRED)

include_directories(include ${GLFW_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})

message(STATUS "GLFW_INCLUDE_DIRS:${GLFW_INCLUDE_DIRS}")
message(STATUS "GLFW_STATIC_LIBRARIES:${GLFW_STATIC_LIBRARIES}")
message(STATUS "Vulkan_INCLUDE_DIR:${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan_LIBRARIES:${Vulkan_LIBRARIES}")

add_subdirectory(example)
