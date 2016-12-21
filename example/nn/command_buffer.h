#pragma once
#include<vulkan/vulkan.hpp>
#include<vector>

namespace simpleVK
{
namespace neuralNetwork
{
class Device;
class Resources;
class Pipeline;

class CommandBuffer
{
	std::shared_ptr<Device> device_;
	std::shared_ptr<Resources> resources_;
	std::shared_ptr<Pipeline> pipeline_;

	vk::Queue queue_;
	vk::CommandPool cmdPool_;
	vk::CommandBuffer cmdBuffer_;

	void getQueue(vk::Queue& queue);
	void createCommandPool(vk::CommandPool& cmdPool);
	void createCommandBuffer(
		const vk::CommandPool& cmdPool,
		vk::CommandBuffer& cmdBuffer);
public:
	explicit CommandBuffer(
		std::shared_ptr<Device> device,
		std::shared_ptr<Resources> resources,
		std::shared_ptr<Pipeline> pipeline);
	~CommandBuffer();

	const vk::CommandBuffer& getCommandBuffers() const;

	void dispach() const;
};
}
}
