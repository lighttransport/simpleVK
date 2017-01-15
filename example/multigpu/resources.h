#pragma once
#include<memory>
#include<vulkan/vulkan.hpp>

namespace simpleVK
{
namespace multiGPU
{
class DevicesManager;
class Window;
class Resources
{
private:
	struct Swapchain
	{
		vk::SurfaceFormatKHR format;
		vk::SwapchainKHR swapchain;
	};
	struct SwapchainImages
	{
		vk::Format format;
		std::vector<vk::Image> images;
		std::vector<vk::ImageView> views;
	};
	struct Vertexes
	{
		vk::DeviceSize size;
		vk::Buffer buffer;
		vk::DeviceMemory memory;
		std::vector<vk::VertexInputAttributeDescription> attributes;
		std::vector<vk::VertexInputBindingDescription> bindings;
	};
	struct Matrix
	{
		vk::DeviceSize size;
		vk::Buffer buffer;
		vk::DeviceMemory memory;
		vk::DescriptorSetLayout layout;
		vk::DescriptorSet set;
	};
	struct Result
	{
		vk::Format format;
		vk::Image image;
		vk::ImageView view;
		vk::DeviceSize size;
		vk::DeviceMemory memory;
	};
	struct CompositedResults
	{
		Result left;
		Result right;
		vk::DescriptorSetLayout layout;
		vk::DescriptorSet set;
	};

	std::shared_ptr<DevicesManager> devices_;
	std::shared_ptr<Window> window_;
	Swapchain swapchain_;
	SwapchainImages swapchainImages_;

	vk::DescriptorPool leftPool_;
	vk::DescriptorPool rightPool_;

	Vertexes leftMesh_;
	Matrix leftMatrix_;

	Vertexes rightMesh_;
	Matrix rightMatrix_;

	Result tmpResult_;
	CompositedResults results_;
	Vertexes compositedMesh_;

	void createSwapchain(
		Swapchain& swapchain);

	void createSwapchainImage(
		const Swapchain& swapchain,
		SwapchainImages& images);

	void createVertexDescription(
			std::vector<vk::VertexInputBindingDescription>& bindings,
			std::vector<vk::VertexInputAttributeDescription>& attributes);

	void createVertexBuffer(size_t gpuIndex, vk::DeviceSize size, vk::Buffer& buffer);

	void createMatrixBuffer(size_t gpuIndex, vk::DeviceSize size, vk::Buffer& buffer);

	void createResultImage(size_t gpuIndex, vk::Image& image);

	void createAndBindMemory(
	  size_t gpuIndex,
		const vk::Buffer & buffer,
		vk::DeviceMemory & memory);

	void createAndBindMemory(
		size_t gpuIndex,
		const vk::Image & image,
		vk::DeviceMemory & memory);

	void createLeftDescriptorPool(vk::DescriptorPool& pool);
	void createRightDescriptorPool(vk::DescriptorPool& pool);

	void createMatrixDescriptorSetLayout(size_t gpuIndex, vk::DescriptorSetLayout& layout);
	void createMatrixDescriptorSet(
			size_t gpuIndex,
			const vk::DescriptorSetLayout& layout,
			const vk::DescriptorPool& pool,
			vk::DescriptorSet& set);

	void createCompositeDescriptorSetLayout(vk::DescriptorSetLayout& layout);
	void createComositeDescriptorSet(
			const vk::DescriptorSetLayout& layout,
			const vk::DescriptorPool& pool,
			vk::DescriptorSet& set);

public:
	Resources(std::shared_ptr<DevicesManager> devices, std::shared_ptr<Window> window);
	size_t getSwapchainCount() const;
	const vk::Image& getSwapchainImage(size_t index) const;
};
}
}
