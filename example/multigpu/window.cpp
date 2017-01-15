#include "window.h"

#include<thread>
#include"devices_manager.h"

simpleVK::multiGPU::Window::Window(
	std::shared_ptr<DevicesManager> devices,
	std::string name,
	uint32_t width,
	uint32_t height,
	std::chrono::milliseconds interval):
	devices_(devices),
	width_(width),
	height_(height),
	interval_(interval)
{
	glfwInit();
	if (!glfwVulkanSupported())
	{
		//TODO exception
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window_ = glfwCreateWindow(width_, height_, name.c_str(), nullptr, nullptr);
	glfwCreateWindowSurface(devices->getInstance(),window_,nullptr,reinterpret_cast<VkSurfaceKHR*>(&surface_));
}

simpleVK::multiGPU::Window::~Window()
{
}

void simpleVK::multiGPU::Window::run(std::function<void()> render)
{
	while (!glfwWindowShouldClose(window_))
	{
		glfwPollEvents();
		render();
		std::this_thread::sleep_for(interval_);
	}
}

void simpleVK::multiGPU::Window::close()
{
	glfwSetWindowShouldClose(window_,true);
}

uint32_t simpleVK::multiGPU::Window::getWidth() const
{
	return width_;
}

uint32_t simpleVK::multiGPU::Window::getHeight() const
{
	return height_;
}

std::chrono::milliseconds simpleVK::multiGPU::Window::getInterval() const
{
	return interval_;
}

const vk::SurfaceKHR & simpleVK::multiGPU::Window::getSurface() const
{
	return surface_;
}

