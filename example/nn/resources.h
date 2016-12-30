#pragma once
#include<vector>
#include<vulkan/vulkan.hpp>

namespace simpleVK
{
namespace neuralNetwork
{
struct LayerSize
{
	size_t x;
	size_t y;
};

class Device;

//Resources, create, read, and write resources (e.g.: Buffer, Image)
class Resources
{
private:
	struct Layer
	{
		LayerSize size;
		vk::DeviceSize bufferSize;
		vk::Buffer buffer;
		vk::DeviceMemory memory;
	};
	struct Weight
	{
		LayerSize inputSize;
		LayerSize outputSize;
		vk::DeviceSize bufferSize;
		vk::Buffer buffer;
		vk::DeviceMemory memory;
	};

	std::shared_ptr<Device> device_;

	std::vector<Layer> layers_;
	std::vector<Weight> weights_;

	vk::DescriptorSetLayout sigmoidSetLayout_;
	vk::DescriptorSetLayout weightSetLayout_;

	vk::DescriptorPool pool_;

	std::vector<vk::DescriptorSet> sigmoidSets_;
	std::vector<vk::DescriptorSet> weightSets_;

	void createBuffer(vk::DeviceSize size,vk::Buffer& inputBuffer);
	void createAndBindMemory(const vk::Buffer& buffer, vk::DeviceMemory& deviceMemory);

	void createSigmoidDescriptorSetLayout(vk::DescriptorSetLayout& setLayout);
	void createWeightDescriptorSetLayout(vk::DescriptorSetLayout& setLayout);

	void createDescriptorPool(
		size_t sigmoidSetsCount,
		size_t weightSetsCount,
		vk::DescriptorPool& pool);
	void createDescriptorSet(
		const vk::DescriptorSetLayout& setLayout,
		const vk::DescriptorPool& pool,
		vk::DescriptorSet& set);
	void writeDescriptorSet(
		const vk::Buffer& buffer,
		const vk::DeviceSize size,
		const vk::DescriptorSet& set,
		const vk::DescriptorType type,
		uint32_t binding);
public:
	explicit Resources(
		std::shared_ptr<Device> device,
		const std::vector<LayerSize>& layerSizes);
	~Resources();

	size_t getLayerCount() const;
	LayerSize getLayerSize(size_t index) const;

	const vk::DescriptorSetLayout& getSigmoidDescriptorSetLayout() const;
	const vk::DescriptorSetLayout& getWeightDescriptorSetLayout() const;

	size_t getSigmoidLayerCount() const;
	size_t getWeightLayerCount() const;

	void getSigmoidLayerSize(size_t index,LayerSize& inputSize,LayerSize& outputSize);
	void getWeightLayerSize(size_t index,LayerSize& inputSize,LayerSize& outputSize);

	const vk::DescriptorSet& getSigmoidDescriptorSet(size_t index) const;
	const vk::DescriptorSet& getWeightDescriptorSet(size_t index) const;

	void writeInputBuffer(const std::vector<float>& input);
	void writeWeightBuffer(size_t index,const std::vector<float>& weight);
	void readOutputBuffer(std::vector<float>& output);
	void readLayerBuffer(size_t index, std::vector<float>& output);

	vk::BufferMemoryBarrier createBarrierWriteToRead(size_t index);
};

}
}
