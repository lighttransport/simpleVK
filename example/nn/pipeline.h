#pragma once
#include<vulkan/vulkan.hpp>

namespace simpleVK
{
	class Device;
	class Resources;

	class Pipeline
	{
		Device& device_;
		Resources& resources_;

		vk::PipelineCache pipelineCache_;
		vk::PipelineLayout pipelineLayout_;
		vk::ShaderModule shaderModule_;
		vk::Pipeline pipeline_;

		void createPipelineCache(vk::PipelineCache& pipelineCache);
		void createPipelineLayout(vk::PipelineLayout& pipelineLayout);
		void createShader(vk::ShaderModule& shader);
		void createPipeline(
			const vk::PipelineCache& pipelineCache,
			const vk::PipelineLayout& pipelineLayout,
			const vk::ShaderModule& shader,
			vk::Pipeline& pipeline);
	public:
		Pipeline(Device& device,Resources& resources);
		~Pipeline();

		const vk::PipelineLayout& getPipelineLayout() const;
		const vk::Pipeline& getPipeline() const;
	};
}
