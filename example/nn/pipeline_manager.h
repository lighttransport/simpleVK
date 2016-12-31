#pragma once
#include<vector>
#include<memory>
#include<vulkan/vulkan.hpp>

#include"resources.h"

namespace simpleVK
{
namespace neuralNetwork
{
class DeviceManager;
class Resources;
class Shader;

class PipelineManager
{
	std::shared_ptr<DeviceManager> device_;
	std::shared_ptr<Resources> resources_;
	std::shared_ptr<Shader> sigmoidShader_;
	std::shared_ptr<Shader> weightShader_;

	vk::PipelineLayout sigmoidPipelineLayout_;
	vk::PipelineLayout weightPipelineLayout_;
	std::vector<vk::Pipeline> sigmoidPipelines_;
	std::vector<vk::Pipeline> weightPipelines_;

	void createPipelineLayout(
		const vk::DescriptorSetLayout& layout,
		vk::PipelineLayout& pipelineLayout);

	void createSigmoidPipeline(
		const vk::PipelineLayout& pipelineLayout,
		const vk::ShaderModule& shader,
		LayerSize inputSize,
		LayerSize outputSize,
		float gain,
		vk::Pipeline& pipeline);

	void createWeightPipeline(
		const vk::PipelineLayout& pipelineLayout,
		const vk::ShaderModule& shader,
		LayerSize inputSize,
		LayerSize outputSize,
		vk::Pipeline& pipeline);
public:
	explicit PipelineManager(
		std::shared_ptr<DeviceManager> device, 
		std::shared_ptr<Resources> resources,
		std::shared_ptr<Shader> sigmoidShader,
		std::shared_ptr<Shader> weightShader);
	~PipelineManager();

	const vk::PipelineLayout& getSigmoidPipelineLayout() const;
	const vk::PipelineLayout& getWeightPipelineLayout() const;
	size_t getSigmoidPipelineCount() const;
	size_t getWeightPipelineCount() const;
	const vk::Pipeline& getSigmoidPipeline(size_t index) const;
	const vk::Pipeline& getWeightPipeline(size_t index) const;
};
}
}
