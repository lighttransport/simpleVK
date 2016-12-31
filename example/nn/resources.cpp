#include "resources.h"

#include"device_manager.h"


void simpleVK::neuralNetwork::Resources::createBuffer(
	vk::DeviceSize size,
	vk::Buffer & buffer)
{
	vk::BufferCreateInfo createInfo;
	createInfo.setSize(size);
	createInfo.setUsage(vk::BufferUsageFlagBits::eStorageBuffer);
	buffer = device_->getDevice().createBuffer(createInfo);

}

void simpleVK::neuralNetwork::Resources::createAndBindMemory(
	const vk::Buffer & buffer,
	vk::DeviceMemory & deviceMemory)
{
	//get PhysicalDeviceMemoryProperties
	vk::PhysicalDeviceMemoryProperties memoryProperties;
	device_->getPhysicalDevice().getMemoryProperties(&memoryProperties);

	//get MemoryRequirements
	vk::MemoryRequirements req;
	device_->getDevice().getBufferMemoryRequirements(buffer, &req);

	uint32_t memoryTypeBits = req.memoryTypeBits;
	uint32_t memoryTypeIndex = 0;
	for (int i = 0; i < (sizeof(memoryTypeBits) * 8); ++i)
	{
		if ((memoryTypeBits >> i) & 1)
		{
			if (memoryProperties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible)
			{
				memoryTypeIndex = i;
				break;
			}
		}
	}

	//init MemoryAllocateInfo
	vk::MemoryAllocateInfo memoryInfo;
	memoryInfo.setMemoryTypeIndex(memoryTypeIndex);
	memoryInfo.setAllocationSize(req.size);

	//allocate VertexMemory
	deviceMemory = device_->getDevice().allocateMemory(memoryInfo);
	//bind VertexMemory to VertexBuffer
	device_->getDevice().bindBufferMemory(buffer, deviceMemory, 0);
}

void simpleVK::neuralNetwork::Resources::createSigmoidDescriptorSetLayout(vk::DescriptorSetLayout & setLayout)
{
	std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings(2);
	setLayoutBindings[0].setBinding(0);
	setLayoutBindings[0].setDescriptorType(vk::DescriptorType::eStorageBuffer);
	setLayoutBindings[0].setDescriptorCount(1);
	setLayoutBindings[0].setStageFlags(vk::ShaderStageFlagBits::eCompute);
	setLayoutBindings[1].setBinding(1);
	setLayoutBindings[1].setDescriptorType(vk::DescriptorType::eStorageBuffer);
	setLayoutBindings[1].setDescriptorCount(1);
	setLayoutBindings[1].setStageFlags(vk::ShaderStageFlagBits::eCompute);

	vk::DescriptorSetLayoutCreateInfo createInfo;
	createInfo.setBindingCount(static_cast<uint32_t>(setLayoutBindings.size()));
	createInfo.setPBindings(setLayoutBindings.data());

	setLayout = device_->getDevice().createDescriptorSetLayout(createInfo);
}

void simpleVK::neuralNetwork::Resources::createWeightDescriptorSetLayout(vk::DescriptorSetLayout & setLayout)
{
	std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings(3);
	setLayoutBindings[0].setBinding(0);
	setLayoutBindings[0].setDescriptorType(vk::DescriptorType::eStorageBuffer);
	setLayoutBindings[0].setDescriptorCount(1);
	setLayoutBindings[0].setStageFlags(vk::ShaderStageFlagBits::eCompute);
	setLayoutBindings[1].setBinding(1);
	setLayoutBindings[1].setDescriptorType(vk::DescriptorType::eStorageBuffer);
	setLayoutBindings[1].setDescriptorCount(1);
	setLayoutBindings[1].setStageFlags(vk::ShaderStageFlagBits::eCompute);
	setLayoutBindings[2].setBinding(2);
	setLayoutBindings[2].setDescriptorType(vk::DescriptorType::eStorageBuffer);
	setLayoutBindings[2].setDescriptorCount(1);
	setLayoutBindings[2].setStageFlags(vk::ShaderStageFlagBits::eCompute);

	vk::DescriptorSetLayoutCreateInfo createInfo;
	createInfo.setBindingCount(static_cast<uint32_t>(setLayoutBindings.size()));
	createInfo.setPBindings(setLayoutBindings.data());

	setLayout = device_->getDevice().createDescriptorSetLayout(createInfo);
}

void simpleVK::neuralNetwork::Resources::createDescriptorPool(
	uint32_t sigmoidSetsCount,
	uint32_t weightSetsCount,
	vk::DescriptorPool& pool)
{
	std::vector<vk::DescriptorPoolSize> poolSizes(1);
	poolSizes[0].setType(vk::DescriptorType::eStorageBuffer);
	poolSizes[0].setDescriptorCount(sigmoidSetsCount * 2 + weightSetsCount * 3);

	vk::DescriptorPoolCreateInfo createInfo;
	createInfo.setMaxSets(sigmoidSetsCount + weightSetsCount);
	createInfo.setPPoolSizes(poolSizes.data());
	createInfo.setPoolSizeCount(static_cast<uint32_t>(poolSizes.size()));
	createInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

	pool = device_->getDevice().createDescriptorPool(createInfo);
}

void simpleVK::neuralNetwork::Resources::createDescriptorSet(
	const vk::DescriptorSetLayout& setLayout,
	const vk::DescriptorPool& pool,
	vk::DescriptorSet& set)
{
	vk::DescriptorSetAllocateInfo allocateInfo;
	allocateInfo.setPSetLayouts(&setLayout);
	allocateInfo.setDescriptorSetCount(1);
	allocateInfo.setDescriptorPool(pool);
	set = device_->getDevice().allocateDescriptorSets(allocateInfo)[0];
}

void simpleVK::neuralNetwork::Resources::writeDescriptorSet(
	const vk::Buffer& buffer,
	const vk::DeviceSize size,
	const vk::DescriptorSet& set,
	vk::DescriptorType type,
	uint32_t binding)
{
	//init DescriptorBufferInfo
	vk::DescriptorBufferInfo bufferInfo;
	bufferInfo.setBuffer(buffer);
	bufferInfo.setOffset(0);
	bufferInfo.setRange(size);

	//init WriteDescriptorSet
	vk::WriteDescriptorSet writeSet;
	writeSet.setDstSet(set);
	writeSet.setDstBinding(binding);
	writeSet.setDstArrayElement(0);
	writeSet.setDescriptorCount(1);
	writeSet.setDescriptorType(type);
	writeSet.setPBufferInfo(&bufferInfo);

	//update DescriptorSet
	device_->getDevice().updateDescriptorSets(writeSet, {});
}

simpleVK::neuralNetwork::Resources::Resources(
	std::shared_ptr<DeviceManager> device,
	const std::vector<LayerSize>& layerSizes) :
	device_(device),
	layers_(layerSizes.size()),
	weights_(layerSizes.size() / 2),
	sigmoidSets_(layerSizes.size() / 2),
	weightSets_(layerSizes.size() / 2)
{
	for (int i = 0; i < layers_.size(); ++i)
	{
		layers_[i].size = layerSizes[i];
		layers_[i].bufferSize = layers_[i].size.y * layers_[i].size.x * sizeof(float);
		createBuffer(layers_[i].bufferSize, layers_[i].buffer);
		createAndBindMemory(layers_[i].buffer, layers_[i].memory);
	}

	for (int i = 0; i < weights_.size(); ++i)
	{
		weights_[i].inputSize = layerSizes[i * 2];
		weights_[i].outputSize = layerSizes[i * 2 + 1];
		weights_[i].bufferSize
			= weights_[i].outputSize.y * weights_[i].outputSize.x
			* weights_[i].inputSize.y * weights_[i].inputSize.x
			* sizeof(float);
		createBuffer(weights_[i].bufferSize, weights_[i].buffer);
		createAndBindMemory(weights_[i].buffer, weights_[i].memory);
	}

	createDescriptorPool(static_cast<uint32_t>(sigmoidSets_.size()), static_cast<uint32_t>(weightSets_.size()), pool_);

	createSigmoidDescriptorSetLayout(sigmoidSetLayout_);
	createWeightDescriptorSetLayout(weightSetLayout_);

	for (int i = 0; i < weightSets_.size(); ++i)
	{
		createDescriptorSet(weightSetLayout_, pool_, weightSets_[i]);

		writeDescriptorSet(
			layers_[2 * i].buffer,
			layers_[2 * i].bufferSize,
			weightSets_[i],
			vk::DescriptorType::eStorageBuffer,
			0);
		writeDescriptorSet(
			layers_[2 * i + 1].buffer,
			layers_[2 * i + 1].bufferSize,
			weightSets_[i],
			vk::DescriptorType::eStorageBuffer,
			1);
		writeDescriptorSet(
			weights_[i].buffer,
			weights_[i].bufferSize,
			weightSets_[i],
			vk::DescriptorType::eStorageBuffer,
			2);
	}

	for (int i = 0; i < sigmoidSets_.size(); ++i)
	{
		createDescriptorSet(sigmoidSetLayout_, pool_, sigmoidSets_[i]);

		writeDescriptorSet(
			layers_[i * 2 + 1].buffer,
			layers_[i * 2 + 1].bufferSize,
			sigmoidSets_[i],
			vk::DescriptorType::eStorageBuffer,
			0);
		writeDescriptorSet(
			layers_[i * 2 + 2].buffer,
			layers_[i * 2 + 2].bufferSize,
			sigmoidSets_[i],
			vk::DescriptorType::eStorageBuffer,
			1);
	}
}

simpleVK::neuralNetwork::Resources::~Resources()
{
	device_->getDevice().freeDescriptorSets(pool_, sigmoidSets_);
	device_->getDevice().freeDescriptorSets(pool_, weightSets_);

	device_->getDevice().destroyDescriptorPool(pool_);

	device_->getDevice().destroyDescriptorSetLayout(sigmoidSetLayout_);
	device_->getDevice().destroyDescriptorSetLayout(weightSetLayout_);

	for (int i = 0; i < layers_.size(); ++i)
	{
		device_->getDevice().freeMemory(layers_[i].memory);
		device_->getDevice().destroyBuffer(layers_[i].buffer);
	}
	for (int i = 0; i < weights_.size(); ++i)
	{
		device_->getDevice().freeMemory(weights_[i].memory);
		device_->getDevice().destroyBuffer(weights_[i].buffer);
	}
}

size_t simpleVK::neuralNetwork::Resources::getLayerCount() const
{
	return layers_.size();
}

simpleVK::neuralNetwork::LayerSize simpleVK::neuralNetwork::Resources::getLayerSize(size_t index) const
{
	return layers_[index].size;
}

size_t simpleVK::neuralNetwork::Resources::getSigmoidLayerCount() const
{
	return sigmoidSets_.size();
}
size_t simpleVK::neuralNetwork::Resources::getWeightLayerCount() const
{
	return weightSets_.size();
}

void simpleVK::neuralNetwork::Resources::getSigmoidLayerSize(size_t index, LayerSize& inputSize, LayerSize& outputSize)
{
	inputSize = layers_[index * 2 + 1].size;
	outputSize = layers_[index * 2 + 2].size;
}

void simpleVK::neuralNetwork::Resources::getWeightLayerSize(size_t index, LayerSize & inputSize, LayerSize & outputSize)
{
	inputSize = layers_[index * 2].size;
	outputSize = layers_[index * 2 + 1].size;
}

const vk::DescriptorSetLayout & simpleVK::neuralNetwork::Resources::getSigmoidDescriptorSetLayout() const
{
	return sigmoidSetLayout_;
}

const vk::DescriptorSetLayout & simpleVK::neuralNetwork::Resources::getWeightDescriptorSetLayout() const
{
	return weightSetLayout_;
}


const vk::DescriptorSet & simpleVK::neuralNetwork::Resources::getSigmoidDescriptorSet(size_t index) const
{
	return sigmoidSets_[index];
}

const vk::DescriptorSet & simpleVK::neuralNetwork::Resources::getWeightDescriptorSet(size_t index) const
{
	return weightSets_[index];
}

void simpleVK::neuralNetwork::Resources::writeInputBuffer(const std::vector<float>& input)
{
	void* mappedMemory = nullptr;

	mappedMemory = device_->getDevice().mapMemory(
		layers_.front().memory,
		0,
		layers_.front().bufferSize);

	memcpy(mappedMemory, input.data(), layers_.front().bufferSize);

	device_->getDevice().unmapMemory(layers_.front().memory);

}

void simpleVK::neuralNetwork::Resources::writeWeightBuffer(
	size_t index,
	const std::vector<float>& weight)
{
	void* mappedMemory = nullptr;

	mappedMemory = device_->getDevice().mapMemory(
		weights_[index].memory,
		0,
		weights_[index].bufferSize);

	memcpy(mappedMemory, weight.data(), weights_[index].bufferSize);

	device_->getDevice().unmapMemory(weights_[index].memory);

}

void simpleVK::neuralNetwork::Resources::readOutputBuffer(std::vector<float>& output)
{
	void* mappedMemory = nullptr;

	mappedMemory = device_->getDevice().mapMemory(
		layers_.back().memory,
		0,
		layers_.back().bufferSize);

	//copy Vertexes
	std::memcpy(output.data(), mappedMemory, layers_.back().bufferSize);

	//unmap VertexMemory
	device_->getDevice().unmapMemory(layers_.back().memory);
}

void simpleVK::neuralNetwork::Resources::readLayerBuffer(size_t index, std::vector<float>& output)
{
	void* mappedMemory = nullptr;

	mappedMemory = device_->getDevice().mapMemory(
		layers_[index].memory,
		0,
		layers_[index].bufferSize);

	//copy Vertexes
	std::memcpy(output.data(), mappedMemory, layers_[index].bufferSize);

	//unmap VertexMemory
	device_->getDevice().unmapMemory(layers_[index].memory);
}

vk::BufferMemoryBarrier simpleVK::neuralNetwork::Resources::createBarrierWriteToRead(size_t index)
{
	vk::BufferMemoryBarrier barrier;
	barrier.setBuffer(layers_[index].buffer);
	barrier.setSize(layers_[index].bufferSize);
	barrier.setOffset(0);
	barrier.setSrcAccessMask(vk::AccessFlagBits::eShaderWrite);
	barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
	barrier.setSrcQueueFamilyIndex(0);
	barrier.setDstQueueFamilyIndex(0);
	return barrier;
}


