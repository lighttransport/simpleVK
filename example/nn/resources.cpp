#include "resources.h"

#include<iostream>
#include"device.h"


void simpleVK::Resources::createInputBuffer(vk::Buffer & inputBuffer)
{
	vk::BufferCreateInfo createInfo;
	createInfo.setSize(inputSize_);
	createInfo.setUsage(vk::BufferUsageFlagBits::eStorageBuffer);
	inputBuffer = device_.getDevice().createBuffer(createInfo);

}

void simpleVK::Resources::createWeightBuffer(vk::Buffer & weightBuffer)
{
	vk::BufferCreateInfo createInfo;
	createInfo.setSize(weightSize_);
	createInfo.setUsage(vk::BufferUsageFlagBits::eStorageBuffer);
	weightBuffer = device_.getDevice().createBuffer(createInfo);
}

void simpleVK::Resources::createOutputBuffer(vk::Buffer & outputBuffer)
{
	vk::BufferCreateInfo createInfo;
	createInfo.setSize(outputSize_);
	createInfo.setUsage(vk::BufferUsageFlagBits::eStorageBuffer);
	outputBuffer = device_.getDevice().createBuffer(createInfo);

}

void simpleVK::Resources::createAndBindMemory(const vk::Buffer & buffer, vk::DeviceMemory & deviceMemory)
{
	//get PhysicalDeviceMemoryProperties
	vk::PhysicalDeviceMemoryProperties memoryProperties;
	device_.getPhysicalDevice().getMemoryProperties(&memoryProperties);

	//get MemoryRequirements
	vk::MemoryRequirements req;
	device_.getDevice().getBufferMemoryRequirements(buffer, &req);

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
	deviceMemory = device_.getDevice().allocateMemory(memoryInfo);
	//bind VertexMemory to VertexBuffer
	device_.getDevice().bindBufferMemory(buffer,deviceMemory,0);
}

void simpleVK::Resources::createDescriptorSetLayout(vk::DescriptorSetLayout & setLayout)
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
	createInfo.setBindingCount(3);
	createInfo.setPBindings(setLayoutBindings.data());

	setLayout = device_.getDevice().createDescriptorSetLayout(createInfo);
}

void simpleVK::Resources::createDescriptorPool(vk::DescriptorPool& pool)
{
  std::vector<vk::DescriptorPoolSize> poolSizes(3);
  poolSizes[0].setType(vk::DescriptorType::eStorageBuffer);
  poolSizes[0].setDescriptorCount(1);
  poolSizes[1].setType(vk::DescriptorType::eStorageBuffer);
  poolSizes[1].setDescriptorCount(1);
  poolSizes[2].setType(vk::DescriptorType::eStorageBuffer);
  poolSizes[2].setDescriptorCount(1);

  vk::DescriptorPoolCreateInfo createInfo;
  createInfo.setMaxSets(1);
  createInfo.setPPoolSizes(poolSizes.data());
  createInfo.setPoolSizeCount(poolSizes.size());
  
  pool = device_.getDevice().createDescriptorPool(createInfo);
}

void simpleVK::Resources::createDescriptorSet(
    const vk::DescriptorSetLayout& setLayout,
    const vk::DescriptorPool& pool,
    vk::DescriptorSet& set)
{
  vk::DescriptorSetAllocateInfo allocateInfo;
  allocateInfo.setPSetLayouts(&setLayout);
  allocateInfo.setDescriptorSetCount(1);
  allocateInfo.setDescriptorPool(pool);
  set = device_.getDevice().allocateDescriptorSets(allocateInfo)[0];
}

void simpleVK::Resources::writeDescriptorSet(const vk::Buffer& buffer,const vk::DeviceSize& size,uint32_t binding)
{
  //init DescriptorBufferInfo
  vk::DescriptorBufferInfo bufferInfo;
  bufferInfo.setBuffer(buffer);
  bufferInfo.setOffset(0);
  bufferInfo.setRange(size);

  //init WriteDescriptorSet
  vk::WriteDescriptorSet writeSet;
  writeSet.setDstSet(set_);
  writeSet.setDstBinding(binding);
  writeSet.setDstArrayElement(0);
  writeSet.setDescriptorCount(1);
  writeSet.setDescriptorType(vk::DescriptorType::eStorageBuffer);
  writeSet.setPBufferInfo(&bufferInfo);

  //update DescriptorSet
  device_.getDevice().updateDescriptorSets(writeSet,{});
}

simpleVK::Resources::Resources(Device & device) :
	device_(device),
	inputSize_(sizeof(float[4*4])),
	weightSize_(sizeof(float[(4*4)*(2*2)])),
	outputSize_(sizeof(float[2*2]))
{
	createInputBuffer(inputBuffer_);
	createAndBindMemory(inputBuffer_, inputMemory_);

	createWeightBuffer(weightBuffer_);
	createAndBindMemory(weightBuffer_, weightMemory_);

	createOutputBuffer(outputBuffer_);
	createAndBindMemory(outputBuffer_, outputMemory_);

  createDescriptorSetLayout(setLayout_);
  createDescriptorPool(pool_);
  createDescriptorSet(setLayout_,pool_,set_);

  writeDescriptorSet(inputBuffer_,inputSize_,0);
  writeDescriptorSet(weightBuffer_,weightSize_,1);
  writeDescriptorSet(outputBuffer_,outputSize_,2);

}

simpleVK::Resources::~Resources()
{
	device_.getDevice().destroyBuffer(inputBuffer_);
	device_.getDevice().freeMemory(inputMemory_);

	device_.getDevice().destroyBuffer(weightBuffer_);
	device_.getDevice().freeMemory(weightMemory_);

	device_.getDevice().destroyBuffer(outputBuffer_);
	device_.getDevice().freeMemory(outputMemory_);
}

const vk::Buffer& simpleVK::Resources::getInputBuffer() const
{
	return inputBuffer_;
}

const vk::Buffer& simpleVK::Resources::getWeightBuffer() const
{
	return weightBuffer_;
}

const vk::Buffer& simpleVK::Resources::getOutputBuffer() const
{
	return outputBuffer_;
}

const vk::DescriptorSetLayout& simpleVK::Resources::getDescriptorSetLayout() const
{
  return setLayout_;
}

const vk::DescriptorSet& simpleVK::Resources::getDescriptorSet() const
{
  return set_;
}

void simpleVK::Resources::writeInputBuffer(const std::vector<float>& input)
{
	void* mappedMemory = nullptr;

	mappedMemory = device_.getDevice().mapMemory(inputMemory_, 0, inputSize_);

	//copy Vertexes
	memcpy(mappedMemory,input.data(),inputSize_);

	//unmap VertexMemory
	device_.getDevice().unmapMemory(inputMemory_);

}

void simpleVK::Resources::writeWeightBuffer(const std::vector<float>& weight)
{
	void* mappedMemory = nullptr;

	mappedMemory = device_.getDevice().mapMemory(weightMemory_, 0, weightSize_);

	//copy Vertexes
	memcpy(mappedMemory,weight.data(),weightSize_);

	//unmap VertexMemory
	device_.getDevice().unmapMemory(weightMemory_);

}

void simpleVK::Resources::readOutputBuffer(std::vector<float>& output)
{
	void* mappedMemory = nullptr;

	mappedMemory = device_.getDevice().mapMemory(outputMemory_, 0, outputSize_);

	//copy Vertexes
	memcpy(output.data(),mappedMemory,outputSize_);

	//unmap VertexMemory
	device_.getDevice().unmapMemory(outputMemory_);

}


