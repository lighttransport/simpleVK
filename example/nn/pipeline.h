#pragma once
#include<vulkan/vulkan.hpp>
#include<vector>

namespace simpleVK
{
namespace neuralNetwork
{
class Device;
class Resources;
class Shader;

class Pipeline
{
	Device& device_;
	Resources& resources_;
	Shader& shader_;

	vk::DescriptorSetLayout setLayout_;
	vk::DescriptorPool pool_;
	vk::DescriptorSet set_;

	vk::DescriptorSet descriptorSet_;
	vk::PipelineCache pipelineCache_;
	vk::PipelineLayout pipelineLayout_;
	vk::Pipeline pipeline_;

	void createDescriptorSet(vk::DescriptorSet&);
	void createPipelineCache(vk::PipelineCache& pipelineCache);
	void createPipelineLayout(vk::PipelineLayout& pipelineLayout);
	void createPipeline(
		const vk::PipelineCache& pipelineCache,
		const vk::PipelineLayout& pipelineLayout,
		const vk::ShaderModule& shader,
		vk::Pipeline& pipeline);
public:
	explicit Pipeline(Device& device, Resources& resources, Shader& shader);
	~Pipeline();

	const vk::PipelineLayout& getPipelineLayout() const;
	const vk::Pipeline& getPipeline() const;
};
}
}
