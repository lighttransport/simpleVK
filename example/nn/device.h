#pragma once
#include<vulkan/vulkan.hpp>

namespace simpleVK
{
	class Device
	{
	private:
		vk::Instance instance_;
		vk::Device device_;
		vk::PhysicalDevice physDevice_;

		void createInstance(vk::Instance& instance);
		void getPhysDevice(const vk::Instance& instance,vk::PhysicalDevice& physDevice);
		void createDevice(const vk::PhysicalDevice& physDevice,vk::Device& device);
	public:
		Device();
		~Device();
		const vk::Instance& getInstance() const;
		const vk::Device& getDevice() const;
		const vk::PhysicalDevice& getPhysicalDevice() const;
	};
}
