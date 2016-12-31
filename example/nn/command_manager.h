#pragma once
#include<vector>
#include<vulkan/vulkan.hpp>

namespace simpleVK
{
namespace neuralNetwork
{
class DeviceManager;
class Resources;
class PipelineManager;

class CommandManager
{
	std::shared_ptr<DeviceManager> device_;
	std::shared_ptr<Resources> resources_;
	std::shared_ptr<PipelineManager> pipeline_;

	vk::Queue queue_;
	vk::CommandPool cmdPool_;
	vk::CommandBuffer cmdBuffer_;

	void getQueue(vk::Queue& queue);
	void createCommandPool(vk::CommandPool& cmdPool);
	void createCommandBuffer(
		const vk::CommandPool& cmdPool,
		vk::CommandBuffer& cmdBuffer);
public:
	explicit CommandManager(
		std::shared_ptr<DeviceManager> device,
		std::shared_ptr<Resources> resources,
		std::shared_ptr<PipelineManager> pipeline);
	~CommandManager();

	const vk::CommandBuffer& getCommandBuffers() const;

	void dispach() const;
};
}
}
