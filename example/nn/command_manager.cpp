#include "command_manager.h"

#include"device_manager.h"
#include"resources.h"
#include"pipeline_manager.h"

void simpleVK::neuralNetwork::CommandManager::getQueue(vk::Queue & queue)
{
	queue = device_->getDevice().getQueue(0, 0);
}

void simpleVK::neuralNetwork::CommandManager::createCommandPool(vk::CommandPool & cmdPool)
{
	//init CommandPoolCreateInfo
	vk::CommandPoolCreateInfo cmdPoolInfo;
	cmdPoolInfo.setQueueFamilyIndex(0);
	cmdPoolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

	//create CommandPool
	cmdPool = device_->getDevice().createCommandPool(cmdPoolInfo);

}

void simpleVK::neuralNetwork::CommandManager::createCommandBuffer(const vk::CommandPool & cmdPool, vk::CommandBuffer& cmdBuffer)
{
	//init CommandBufferAllocateInfo
	vk::CommandBufferAllocateInfo cmdBufAllocateInfo;
	cmdBufAllocateInfo.setCommandPool(cmdPool);
	cmdBufAllocateInfo.setLevel(vk::CommandBufferLevel::ePrimary);
	cmdBufAllocateInfo.setCommandBufferCount(1);

	//allocate CommandBuffer
	cmdBuffer = (device_->getDevice().allocateCommandBuffers(cmdBufAllocateInfo))[0];
}

simpleVK::neuralNetwork::CommandManager::CommandManager(
	std::shared_ptr<DeviceManager> device,
	std::shared_ptr<Resources> resources,
	std::shared_ptr<PipelineManager> pipeline) :
	device_(device),
	resources_(resources),
	pipeline_(pipeline)
{
	getQueue(queue_);
	createCommandPool(cmdPool_);
	createCommandBuffer(cmdPool_, cmdBuffer_);
}

simpleVK::neuralNetwork::CommandManager::~CommandManager()
{
	device_->getDevice().freeCommandBuffers(cmdPool_, 1, &cmdBuffer_);
	device_->getDevice().destroyCommandPool(cmdPool_);
}

const vk::CommandBuffer & simpleVK::neuralNetwork::CommandManager::getCommandBuffers() const
{
	return cmdBuffer_;
}

void simpleVK::neuralNetwork::CommandManager::dispach() const
{
	//init CommandBufferInheritanceInfo
	vk::CommandBufferInheritanceInfo inheritanceInfo;
	inheritanceInfo.setPipelineStatistics(vk::QueryPipelineStatisticFlagBits::eComputeShaderInvocations);
	inheritanceInfo.setQueryFlags(vk::QueryControlFlagBits::ePrecise);

	//init CommandBufferBeginInfo
	vk::CommandBufferBeginInfo beginInfo;
	beginInfo.setPInheritanceInfo(&inheritanceInfo);

	//begin CommandBuffer
	cmdBuffer_.begin(&beginInfo);
	for (int i = 0; i < pipeline_->getSigmoidPipelineCount(); ++i)
	{
		//bind Pipeline
		cmdBuffer_.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline_->getWeightPipeline(i));

		//bind DescriptorSet
		cmdBuffer_.bindDescriptorSets(
			vk::PipelineBindPoint::eCompute,
			pipeline_->getWeightPipelineLayout(),
			0,
			resources_->getWeightDescriptorSet(i), {});

		//dispatch SigmoidLayer
		cmdBuffer_.dispatch(1, 1, 1);

		//memory barrier
		vk::BufferMemoryBarrier weightOutputBarrier = resources_->createBarrierWriteToRead(2 * i + 1);
		cmdBuffer_.pipelineBarrier(
			vk::PipelineStageFlagBits::eComputeShader,
			vk::PipelineStageFlagBits::eComputeShader,
			vk::DependencyFlagBits::eByRegion,
			nullptr,
			weightOutputBarrier,
			nullptr);

		//bind Pipeline
		cmdBuffer_.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline_->getSigmoidPipeline(i));

		//bind DescriptorSet
		cmdBuffer_.bindDescriptorSets(
			vk::PipelineBindPoint::eCompute,
			pipeline_->getSigmoidPipelineLayout(),
			0,
			resources_->getSigmoidDescriptorSet(i), {});

		//dispatch WeightLayer
		cmdBuffer_.dispatch(1, 1, 1);

		//memory barrier
		vk::BufferMemoryBarrier sigmoidOutputBarrier = resources_->createBarrierWriteToRead(2 * i + 2);
		cmdBuffer_.pipelineBarrier(
			vk::PipelineStageFlagBits::eComputeShader,
			vk::PipelineStageFlagBits::eComputeShader,
			vk::DependencyFlagBits::eByRegion,
			nullptr,
			sigmoidOutputBarrier,
			nullptr);
	}

	//end CommandBuffer
	cmdBuffer_.end();

	vk::SubmitInfo submitInfo;
	submitInfo.setPCommandBuffers(&cmdBuffer_);
	submitInfo.setCommandBufferCount(1);
	vk::Fence fence;
	queue_.submit(submitInfo, fence);

	//wait Queue
	queue_.waitIdle();

	//wait Device
	device_->getDevice().waitIdle();
}
