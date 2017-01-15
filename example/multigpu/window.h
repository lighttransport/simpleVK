#pragma once
#include<string>
#include<memory>
#include<chrono>
#include<functional>
#include<vulkan/vulkan.hpp>
#include<GLFW/glfw3.h>

namespace simpleVK
{
namespace multiGPU
{
class DevicesManager;

class Window
{
private:
	std::shared_ptr<DevicesManager> devices_;
	uint32_t width_;
	uint32_t height_;
	std::chrono::milliseconds interval_;
	std::function<void(Window&)> render_;

	GLFWwindow* window_;
	vk::SurfaceKHR surface_;
public:
	Window(
		std::shared_ptr<DevicesManager> devices,
		std::string name,
		uint32_t width,
		uint32_t height,
		std::chrono::milliseconds interval);
	~Window();
	void run(std::function<void()> render);
	void close();
	uint32_t getWidth() const;
	uint32_t getHeight() const;
	std::chrono::milliseconds getInterval() const;
	const vk::SurfaceKHR& getSurface() const;
};
}
}
