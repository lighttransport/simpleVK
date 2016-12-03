#pragma once
#include<vector>
#include<vulkan/vulkan.hpp>

namespace simpleVK
{
	class Device;

	class Resources
	{
		Device& device_;

		vk::DeviceSize inputSize_;
		vk::Buffer inputBuffer_;
		vk::DeviceMemory inputMemory_;

		vk::DeviceSize weightSize_;
		vk::Buffer weightBuffer_;
		vk::DeviceMemory weightMemory_;

		vk::DeviceSize outputSize_;
		vk::Buffer outputBuffer_;
		vk::DeviceMemory outputMemory_;

		vk::DescriptorSetLayout setLayout_;
    vk::DescriptorPool pool_;
    vk::DescriptorSet set_;

		void createInputBuffer(vk::Buffer& inputBuffer);
		void createWeightBuffer(vk::Buffer& weightBuffer);
		void createOutputBuffer(vk::Buffer& outputBuffer);
		void createAndBindMemory(const vk::Buffer& buffer,vk::DeviceMemory& deviceMemory);
		void createDescriptorSetLayout(vk::DescriptorSetLayout& setLayout);
    void createDescriptorPool(vk::DescriptorPool& pool);
    void createDescriptorSet(
        const vk::DescriptorSetLayout& setLayout,
        const vk::DescriptorPool& pool,
        vk::DescriptorSet& set);
    void writeDescriptorSet(const vk::Buffer& buffer,const vk::DeviceSize& size,uint32_t binding);
	public:
		Resources(Device& device);
		~Resources();

		const vk::Buffer& getInputBuffer() const;
		const vk::Buffer& getWeightBuffer() const;
		const vk::Buffer& getOutputBuffer() const;

    const vk::DescriptorSetLayout& getDescriptorSetLayout() const;
    const vk::DescriptorSet& getDescriptorSet() const;

		void writeInputBuffer(const std::vector<float>& input);
		void writeWeightBuffer(const std::vector<float>& weight);
		void readOutputBuffer(std::vector<float>& output);
	};
}