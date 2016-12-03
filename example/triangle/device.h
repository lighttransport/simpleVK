#pragma once

#include<vector>
#include<vulkan/vulkan.hpp>

#include"instance.h"

namespace simpleVulkan
{
    class Device
    { 
   private:
       std::vector<vk::PhysicalDevice> m_physicalDevices;
       std::vector<vk::Device> m_devices;
   public:
       Device();
       ~Device();

       Result create(vk::Instance instance, bool validate);
       void destroy();

	   vk::PhysicalDevice& getVkPhysicalDevice(size_t index);
       vk::Device& getVkDevice(size_t index);
    };
}
