#include"device.h"

#include<iostream>
#include<vector>
#include<GLFW/glfw3.h>

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT, uint64_t object,
	size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
	// OutputDebugString(L"Message Code: "); OutputDebugString(std::to_wstring(messageCode).c_str()); OutputDebugString(L"\n");
	std::cout << pLayerPrefix << '[' << messageCode << "]:" << pMessage << std::endl;
	return VK_FALSE;
}

void simpleVK::neuralNetwork::Device::createInstance(vk::Instance& instance)
{
	//init ApplicationInfo
	vk::ApplicationInfo appInfo;
	appInfo.setPApplicationName("NeuralNetwork");
	appInfo.setApplicationVersion(1);
	appInfo.setPEngineName("SimpleVK");
	appInfo.setEngineVersion(1);
	appInfo.setApiVersion(VK_API_VERSION_1_0);

	std::vector<const char*> extensions;
	unsigned int glfwExtensionsCount;
	const char * *  glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
	extensions.resize(glfwExtensionsCount);
	for (size_t i = 0; i < glfwExtensionsCount; ++i)
	{
		extensions[i] = glfwExtensions[i];
	}
	extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	std::vector<const char*> layers;
	layers.push_back("VK_LAYER_LUNARG_standard_validation");

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
void simpleVK::neuralNetwork::Device::createDebugReportCallback(
	const vk::Instance & instance,
	const PFN_vkDebugReportCallbackEXT & callback,
	vk::DebugReportCallbackEXT& reportCallback)
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
void simpleVK::neuralNetwork::Device::getPhysDevice(const vk::Instance& instance, vk::PhysicalDevice & physDevice)
{
	//get PhysicalDevices
	std::vector<vk::PhysicalDevice> physDevices;
	physDevices = instance.enumeratePhysicalDevices();

	physDevice = physDevices[0];
}
void simpleVK::neuralNetwork::Device::createDevice(const vk::PhysicalDevice & physDevice, vk::Device & device)
{
	//create Devices

	//get QueueFamilyProperties
	uint32_t queueFamilyPropertiesCount;
	physDevice.getQueueFamilyProperties(&queueFamilyPropertiesCount, nullptr);
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties(queueFamilyPropertiesCount);
	physDevice.getQueueFamilyProperties(&queueFamilyPropertiesCount, queueFamilyProperties.data());

	uint32_t familyIndex = 0;
	for (int i = 0; i < queueFamilyProperties.size(); ++i)
	{
		if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
		{
			familyIndex = i;
			break;
		}
	}

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
	if (true)	//TODO add macro
	{
		layers.push_back("VK_LAYER_LUNARG_standard_validation");
	}

	deviceInfo.setEnabledExtensionCount(extensions.size());
	deviceInfo.setPpEnabledExtensionNames(extensions.data());

	deviceInfo.setEnabledLayerCount(layers.size());
	deviceInfo.setPpEnabledLayerNames(layers.data());

	//create Device
	physDevice.createDevice(&deviceInfo, nullptr, &device);
	return;
}
simpleVK::neuralNetwork::Device::Device()
{
	createInstance(instance_);
	createDebugReportCallback(instance_,debugCallback,reportCallback_);
	getPhysDevice(instance_, physDevice_);
	createDevice(physDevice_, device_);
}
simpleVK::neuralNetwork::Device::~Device()
{
	device_.destroy();
	_vkDestroyDebugReportCallbackEXT(instance_,reportCallback_,nullptr);
	instance_.destroy();
}
const vk::Instance& simpleVK::neuralNetwork::Device::getInstance() const
{
	return instance_;
}
const vk::Device& simpleVK::neuralNetwork::Device::getDevice() const
{
	return device_;
}
const vk::PhysicalDevice& simpleVK::neuralNetwork::Device::getPhysicalDevice() const
{
	return physDevice_;
}
