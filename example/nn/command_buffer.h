#pragma once
#include<vulkan/vulkan.hpp>
#include<vector>

namespace simpleVK
{
	class Device;
	class Resources;
	class Pipeline;

	class CommandBuffer
	{
		Device& device_;
		Resources& resources_;
		Pipeline& pipeline_;

		vk::Queue queue_;
		vk::CommandPool cmdPool_;
	  vk::CommandBuffer cmdBuffer_;

		void getQueue(vk::Queue& queue);
		void createCommandPool(vk::CommandPool& cmdPool);
		void createCommandBuffer(
			const vk::CommandPool& cmdPool,
			vk::CommandBuffer& cmdBuffer);
	public:
		explicit CommandBuffer(Device& device, Resources& resources, Pipeline& pipeline);
		~CommandBuffer();

		const vk::CommandBuffer& getCommandBuffers() const;

    void dispach() const;
	};
}
