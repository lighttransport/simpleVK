#include"device.h"

#include<iostream>
#include<vector>
#include<GLFW/glfw3.h>

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
  const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
  extensions.resize(glfwExtensionsCount);
  for(size_t i=0;i<glfwExtensionsCount;++i)
  {
    extensions[i]=glfwExtensions[i];
  }

	//std::vector<const char*> layers;
	//layers.push_back("VK_LAYER_LUNARG_param_checker");
	//layers.push_back("VK_LAYER_LUNARG_swapchain");
	//layers.push_back("VK_LAYER_LUNARG_mem_tracker");
	//layers.push_back("VK_LAYER_GOOGLE_unique_objects");
	//deviceInfo.enabledLayerCount(layers.size());
	//deviceInfo.ppEnabledLayerNames(layers.data());

	//init InstanceCreateInfo
	vk::InstanceCreateInfo instInfo;
	instInfo.setPApplicationInfo(&appInfo);
	instInfo.setEnabledExtensionCount(extensions.size());
	instInfo.setPpEnabledExtensionNames(extensions.data());
	//instInfo.setEnabledLayerCount(layers.size());
	//instInfo.setPpEnabledLayerNames(layers.data());

	//create Instance
	instance = vk::createInstance(instInfo);
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

	uint32_t familyIndex;
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
	float queueProperties = 0.0f ;
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
	deviceInfo.setEnabledExtensionCount(extensions.size());
	deviceInfo.setPpEnabledExtensionNames(extensions.data());


	if (true)	//TODO add macro
	{
		layers.push_back("VK_LAYER_LUNARG_standard_validation");
	}

	deviceInfo.setEnabledLayerCount(layers.size());
	deviceInfo.setPpEnabledLayerNames(layers.data());

	//create Device
	physDevice.createDevice(&deviceInfo, nullptr, &device);
	return;
}
simpleVK::neuralNetwork::Device::Device()
{
	createInstance(instance_);
	getPhysDevice(instance_, physDevice_);
	createDevice(physDevice_, device_);
}
simpleVK::neuralNetwork::Device::~Device()
{
	device_.destroy();
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
