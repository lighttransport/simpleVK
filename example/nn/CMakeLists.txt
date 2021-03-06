cmake_minimum_required(VERSION 2.8)

file(GLOB SRCS *.cpp *.h)
file(GLOB SHADER_SRCS *.comp)

foreach(SHADER_SRC IN LISTS SHADER_SRCS)
	get_filename_component(SHADER_NAME ${SHADER_SRC} NAME_WE)
	set(SHADER_BIN "${SHADER_NAME}.spv")
	set(SHADER_BINS ${SHADER_BINS} ${SHADER_BIN}) 
	add_custom_command(OUTPUT ${SHADER_BIN} COMMAND glslangValidator -V ${SHADER_SRC} -o ${SHADER_BIN} MAIN_DEPENDENCY ${SHADER_SRC})
endforeach(SHADER_SRC)

set(TARGET_NAME NeuralNetwork)

add_executable(${TARGET_NAME} ${SRCS} ${SHADER_BINS})

target_link_libraries(${TARGET_NAME} ${GLFW_STATIC_LIBRARIES} ${Vulkan_LIBRARIES}) 

if(MSVC)
	set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})
	set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})
endif(MSVC)

