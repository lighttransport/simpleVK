#pragma once
#include<vulkan/vulkan.hpp>
#include<vector>

#include<memory>

namespace simpleVK
{
namespace neuralNetwork
{
class Device;
class Resources;
class Shader;

class Pipeline
{
	std::shared_ptr<Device> device_;
	std::shared_ptr<Resources> resources_;
	std::shared_ptr<Shader> shader_;

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
	explicit Pipeline(
		std::shared_ptr<Device> device, 
		std::shared_ptr<Resources> resources,
		std::shared_ptr<Shader> shader);
	~Pipeline();

	const vk::PipelineLayout& getPipelineLayout() const;
	const vk::Pipeline& getPipeline() const;
};
}
}
