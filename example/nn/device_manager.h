#pragma once
#include<vulkan/vulkan.hpp>

namespace simpleVK
{
namespace neuralNetwork
{
class DeviceManager
{
private:
	vk::Instance instance_;
	vk::Device device_;
	vk::PhysicalDevice physDevice_;

	vk::DebugReportCallbackEXT reportCallback_;

	PFN_vkCreateDebugReportCallbackEXT _vkCreateDebugReportCallbackEXT;
	PFN_vkDebugReportMessageEXT _vkDebugReportMessageEXT;
	PFN_vkDestroyDebugReportCallbackEXT _vkDestroyDebugReportCallbackEXT;

	void createInstance(vk::Instance& instance);
	void createDebugReportCallback(
		const vk::Instance& instance,
		const PFN_vkDebugReportCallbackEXT& callback,
		vk::DebugReportCallbackEXT& reportCallback);
	void getPhysDevice(const vk::Instance& instance, vk::PhysicalDevice& physDevice);
	void createDevice(const vk::PhysicalDevice& physDevice, vk::Device& device);
public:
	explicit DeviceManager();
	~DeviceManager();
	const vk::Instance& getInstance() const;
	const vk::Device& getDevice() const;
	const vk::PhysicalDevice& getPhysicalDevice() const;
};
}
}