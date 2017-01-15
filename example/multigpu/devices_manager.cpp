#include "devices_manager.h"
#include<iostream>
#include<algorithm>
#include<GLFW/glfw3.h>

#ifdef USE_VALIDATION_LAYER
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT, uint64_t object,
	size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
	// OutputDebugString(L"Message Code: "); OutputDebugString(std::to_wstring(messageCode).c_str()); OutputDebugString(L"\n");
	std::cout << pLayerPrefix << '[' << messageCode << "]:" << pMessage << std::endl;
	return VK_FALSE;
}
#endif // USE_VALIDATION_LAYER

void simpleVK::multiGPU::DevicesManager::createInstance(vk::Instance & instance)
{
	//init ApplicationInfo
	vk::ApplicationInfo appInfo;
	appInfo.setPApplicationName("MultiGPU");
	appInfo.setApplicationVersion(1);
	appInfo.setPEngineName("SimpleVK");
	appInfo.setEngineVersion(1);
	appInfo.setApiVersion(VK_API_VERSION_1_0);

	std::vector<const char*> extensions;
	std::vector<const char*> layers;

	unsigned int glfwExtensionsCount;
	const char * *  glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
	extensions.resize(glfwExtensionsCount);
	for (size_t i = 0; i < glfwExtensionsCount; ++i)
	{
		extensions[i] = glfwExtensions[i];
	}

#ifdef USE_VALIDATION_LAYER
	extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif // USE_VALIDATION_LAYER

	//init InstanceCreateInfo
	vk::InstanceCreateInfo instInfo;
	instInfo.setPApplicationInfo(&appInfo);
	instInfo.setEnabledExtensionCount(extensions.size());
	instInfo.setPpEnabledExtensionNames(extensions.data());
	instInfo.setEnabledLayerCount(layers.size());
	instInfo.setPpEnabledLayerNames(layers.data());

	//create Instance
	instance = vk::createInstance(instInfo);
}

#ifdef USE_VALIDATION_LAYER
void simpleVK::multiGPU::DevicesManager::createDebugReportCallback(const vk::Instance & instance, const PFN_vkDebugReportCallbackEXT & callback, vk::DebugReportCallbackEXT & reportCallback)
{	
	_vkCreateDebugReportCallbackEXT = 
		reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
			instance_.getProcAddr("vkCreateDebugReportCallbackEXT"));
	_vkDebugReportMessageEXT = 
		reinterpret_cast<PFN_vkDebugReportMessageEXT>(
			instance_.getProcAddr("vkDebugReportMessageEXT"));
	_vkDestroyDebugReportCallbackEXT = 
		reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
			instance_.getProcAddr("vkDestroyDebugReportCallbackEXT"));

	vk::DebugReportCallbackCreateInfoEXT callbackInfo;
	callbackInfo.setFlags(
		vk::DebugReportFlagBitsEXT::eError |
		vk::DebugReportFlagBitsEXT::eWarning |
		vk::DebugReportFlagBitsEXT::ePerformanceWarning
	);
	callbackInfo.setPfnCallback(callback);
	_vkCreateDebugReportCallbackEXT(
		instance,
		reinterpret_cast<VkDebugReportCallbackCreateInfoEXT*>(&callbackInfo),
		nullptr,
		reinterpret_cast<VkDebugReportCallbackEXT*>(&reportCallback)); 
}
#endif // USE_VALIDATION_LAYER

void simpleVK::multiGPU::DevicesManager::getPhysDevices(const vk::Instance & instance, vk::PhysicalDevice & mainPhysDevice, vk::PhysicalDevice & subPhysDevice)
{
	//get PhysicalDevices
	std::vector<vk::PhysicalDevice> physDevices;
	physDevices = instance.enumeratePhysicalDevices();

	mainPhysDevice = physDevices[0];
	//subPhysDevice = physDevice[1]; ///TODO
}

void simpleVK::multiGPU::DevicesManager::createDevice(const vk::PhysicalDevice & physDevice, vk::Device & device)
{
	//create Devices

	//get QueueFamilyProperties
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physDevice.getQueueFamilyProperties();

	auto familyIterator = std::find_if(queueFamilyProperties.cbegin(), queueFamilyProperties.cend(), [](auto property)
	{
		return property.queueFlags & vk::QueueFlagBits::eGraphics;
	});
	uint32_t familyIndex = static_cast<uint32_t>(std::distance(queueFamilyProperties.cbegin(), familyIterator));

	//init DeviceQueueInfo
	vk::DeviceQueueCreateInfo queueInfo;
	float queueProperties = 0.0f;
	queueInfo.setQueueFamilyIndex(familyIndex);
	queueInfo.setQueueCount(1);
	queueInfo.setPQueuePriorities(&queueProperties);

	//init DeviceCreateInfo
	vk::DeviceCreateInfo deviceInfo;
	deviceInfo.setQueueCreateInfoCount(1);
	deviceInfo.setPQueueCreateInfos(&queueInfo);

	std::vector<const char*> layers;
	std::vector<const char*> extensions;

	extensions.push_back("VK_KHR_swapchain");
#ifdef USE_VALIDATION_LAYER
	layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif // USE_VALIDATION_LAYER

	deviceInfo.setEnabledExtensionCount(extensions.size());
	deviceInfo.setPpEnabledExtensionNames(extensions.data());

	deviceInfo.setEnabledLayerCount(layers.size());
	deviceInfo.setPpEnabledLayerNames(layers.data());

	//create Device
	device = physDevice.createDevice(deviceInfo);
	return;
}

simpleVK::multiGPU::DevicesManager::DevicesManager()
{
	createInstance(instance_);
#ifdef USE_VALIDATION_LAYER
	createDebugReportCallback(instance_,debugCallback,reportCallback_);
#endif // USE_VALIDATION_LAYER
	getPhysDevices(instance_, mainPhysDevice_,subPhysDevice_);
	createDevice(mainPhysDevice_, mainDevice_);
	//createDevice(subPhysDevice_, subDevice_); //TODO
}

simpleVK::multiGPU::DevicesManager::~DevicesManager()
{
	subDevice_.destroy();
	mainDevice_.destroy();
#ifdef USE_VALIDATION_LAYER
	_vkDestroyDebugReportCallbackEXT(instance_,reportCallback_,nullptr);
#endif // USE_VALIDATION_LAYER
	instance_.destroy();
}

const vk::Instance& simpleVK::multiGPU::DevicesManager::getInstance() const
{
	return instance_;
}
const vk::Device& simpleVK::multiGPU::DevicesManager::getDevice(size_t index) const
{
	return index == 0 ? mainDevice_ : subDevice_;
}
const vk::PhysicalDevice& simpleVK::multiGPU::DevicesManager::getPhysicalDevice(size_t index) const
{
	return index == 0 ? mainPhysDevice_ : subPhysDevice_;
}
