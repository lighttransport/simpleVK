#pragma once
#include<vulkan/vulkan.hpp>

namespace simpleVK
{
namespace multiGPU
{
class DevicesManager
{
private:
	vk::Instance instance_;
	vk::PhysicalDevice mainPhysDevice_;
	vk::PhysicalDevice subPhysDevice_;
	vk::Device mainDevice_;
	vk::Device subDevice_;

#ifdef USE_VALIDATION_LAYER
	vk::DebugReportCallbackEXT reportCallback_;

	PFN_vkCreateDebugReportCallbackEXT _vkCreateDebugReportCallbackEXT;
	PFN_vkDebugReportMessageEXT _vkDebugReportMessageEXT;
	PFN_vkDestroyDebugReportCallbackEXT _vkDestroyDebugReportCallbackEXT;

	PFN_vkCreateDebugReportCallbackEXT _vkCreateDebugReportCallbackEXT;
	PFN_vkDebugReportMessageEXT _vkDebugReportMessageEXT;
	PFN_vkDestroyDebugReportCallbackEXT _vkDestroyDebugReportCallbackEXT;
#endif // USE_VALIDATION_LAYER

	void createInstance(vk::Instance& instance);
#ifdef USE_VALIDATION_LAYER
	void createDebugReportCallback(
		const vk::Instance& instance,
		const PFN_vkDebugReportCallbackEXT& callback,
		vk::DebugReportCallbackEXT& reportCallback);
#endif // USE_VALIDATION_LAYER
	void getPhysDevices(const vk::Instance& instance, vk::PhysicalDevice& mainPhysDevice,vk::PhysicalDevice& subPhysDevice);
	void createDevice(const vk::PhysicalDevice& physDevice, vk::Device& device);
public:
	explicit DevicesManager();
	~DevicesManager();
	const vk::Instance& getInstance() const;
	const vk::Device& getDevice(size_t index) const;
	const vk::PhysicalDevice& getPhysicalDevice(size_t index) const;
};
}
}
