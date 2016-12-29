#include "pipeline.h"

#include"utility.h"
#include"device.h"
#include"resources.h"
#include"shader.h"

void simpleVK::neuralNetwork::Pipeline::createPipelineLayout(
	const vk::DescriptorSetLayout& layout,
	vk::PipelineLayout & pipelineLayout)
{
	//init PipelineLayoutCreateInfo
	vk::PipelineLayoutCreateInfo layoutInfo;
	layoutInfo.setPushConstantRangeCount(0);
	layoutInfo.setPPushConstantRanges(nullptr);
	layoutInfo.setSetLayoutCount(1);
	layoutInfo.setPSetLayouts(&layout);

	//create PipelineLayout
	pipelineLayout = device_->getDevice().createPipelineLayout(layoutInfo);
}

void simpleVK::neuralNetwork::Pipeline::createSigmoidPipeline(
	const vk::PipelineLayout & pipelineLayout,
	const vk::ShaderModule& shader,
	simpleVK::neuralNetwork::LayerSize inputSize,
	simpleVK::neuralNetwork::LayerSize outputSize,
	float gain,
	vk::Pipeline & pipeline)
{
	std::vector<uint32_t> constants(5);
	constants[0] = inputSize.x;
	constants[1] = inputSize.y;
	constants[2] = outputSize.x;
	constants[3] = outputSize.y;
	reinterpret_cast<float&>(constants[4]) = gain;

	std::vector<vk::SpecializationMapEntry> constantEntries(4);
	constantEntries[0].setConstantID(0);
	constantEntries[0].setSize(sizeof(uint32_t));
	constantEntries[0].setOffset(0);
	constantEntries[1].setConstantID(1);
	constantEntries[1].setSize(sizeof(uint32_t));
	constantEntries[1].setOffset(sizeof(uint32_t));
	constantEntries[2].setConstantID(2);
	constantEntries[2].setSize(sizeof(uint32_t));
	constantEntries[2].setOffset(sizeof(uint32_t)*2);
	constantEntries[3].setConstantID(3);
	constantEntries[3].setSize(sizeof(uint32_t));
	constantEntries[3].setOffset(sizeof(uint32_t)*3);
	constantEntries[4].setSize(sizeof(float));
	constantEntries[4].setOffset(sizeof(uint32_t)*4);


	//init SpecializationInfo
	vk::SpecializationInfo specializaionInfo;
	specializaionInfo.setPData(constants.data());
	specializaionInfo.setDataSize(sizeof(uint32_t) * constants.size());
	specializaionInfo.setPMapEntries(constantEntries.data());
	specializaionInfo.setMapEntryCount(constantEntries.size());

	//init PipelineShaderStageCreateInfo
	vk::PipelineShaderStageCreateInfo stageInfo;
	stageInfo.setStage(vk::ShaderStageFlagBits::eCompute);
	stageInfo.setModule(shader);
	stageInfo.setPName("main");
	stageInfo.setPSpecializationInfo(&specializaionInfo);

	//init ComputePipelineCreateInfo
	vk::ComputePipelineCreateInfo createInfo;
	createInfo.setStage(stageInfo);
	createInfo.setLayout(pipelineLayout);

	//init PiplineCache
	vk::PipelineCache cache;

	//create ComputePipeline
	pipeline = device_->getDevice().createComputePipeline(cache, createInfo);
}

void simpleVK::neuralNetwork::Pipeline::createWeightPipeline(
	const vk::PipelineLayout & pipelineLayout,
	const vk::ShaderModule& shader,
	simpleVK::neuralNetwork::LayerSize inputSize,
	simpleVK::neuralNetwork::LayerSize outputSize,
	vk::Pipeline & pipeline)
{
	std::vector<uint32_t> constants(4);
	constants[0] = inputSize.x;
	constants[1] = inputSize.y;
	constants[2] = outputSize.x;
	constants[3] = outputSize.y;

	std::vector<vk::SpecializationMapEntry> constantEntries(4);
	constantEntries[0].setConstantID(0);
	constantEntries[0].setSize(sizeof(uint32_t));
	constantEntries[0].setOffset(0);
	constantEntries[1].setConstantID(1);
	constantEntries[1].setSize(sizeof(uint32_t));
	constantEntries[1].setOffset(sizeof(uint32_t));
	constantEntries[2].setConstantID(2);
	constantEntries[2].setSize(sizeof(uint32_t));
	constantEntries[2].setOffset(sizeof(uint32_t)*2);
	constantEntries[3].setConstantID(3);
	constantEntries[3].setSize(sizeof(uint32_t));
	constantEntries[3].setOffset(sizeof(uint32_t)*3);

	//init SpecializationInfo
	vk::SpecializationInfo specializaionInfo;
	specializaionInfo.setPData(constants.data());
	specializaionInfo.setDataSize(sizeof(uint32_t) * constants.size());
	specializaionInfo.setPMapEntries(constantEntries.data());
	specializaionInfo.setMapEntryCount(constantEntries.size());

	//init PipelineShaderStageCreateInfo
	vk::PipelineShaderStageCreateInfo stageInfo;
	stageInfo.setStage(vk::ShaderStageFlagBits::eCompute);
	stageInfo.setModule(shader);
	stageInfo.setPName("main");
	stageInfo.setPSpecializationInfo(&specializaionInfo);

	//init ComputePipelineCreateInfo
	vk::ComputePipelineCreateInfo createInfo;
	createInfo.setStage(stageInfo);
	createInfo.setLayout(pipelineLayout);

	//init PiplineCache
	vk::PipelineCache cache;

	//create ComputePipeline
	pipeline = device_->getDevice().createComputePipeline(cache, createInfo);
}

simpleVK::neuralNetwork::Pipeline::Pipeline(
	std::shared_ptr<Device> device,
	std::shared_ptr<Resources> resources,
	std::shared_ptr<Shader> sigmoidShader,
	std::shared_ptr<Shader> weightShader) :
	device_(device),
	resources_(resources),
	sigmoidShader_(sigmoidShader),
	weightShader_(weightShader),
	sigmoidPipelines_(resources_->getSigmoidLayerCount()),
	weightPipelines_(resources_->getWeightLayerCount())
{
	createPipelineLayout(resources_->getSigmoidDescriptorSetLayout(),sigmoidPipelineLayout_);
	createPipelineLayout(resources_->getWeightDescriptorSetLayout(),weightPipelineLayout_);

	LayerSize inputSize, outputSize;
	for(int i = 0;i<sigmoidPipelines_.size();++i)
	{
		resources_->getSigmoidLayerSize(i, inputSize, outputSize);
		createSigmoidPipeline(
			sigmoidPipelineLayout_,
			sigmoidShader_->getShaderModule(),
			inputSize,
			outputSize,
			1.0f,
			sigmoidPipelines_[i]);
	}
	for (int i = 0; i < weightPipelines_.size(); ++i)
	{
		resources_->getWeightLayerSize(i, inputSize, outputSize);
		createWeightPipeline(
			weightPipelineLayout_,
			weightShader_->getShaderModule(),
			inputSize,
			outputSize,
			weightPipelines_[i]);
	}

}

simpleVK::neuralNetwork::Pipeline::~Pipeline()
{
	for (int i = 0; i < sigmoidPipelines_.size(); ++i)
	{
		device_->getDevice().destroyPipeline(sigmoidPipelines_[i]);
	}
	for (int i = 0; i < weightPipelines_.size(); ++i)
	{
		device_->getDevice().destroyPipeline(weightPipelines_[i]);
	}
	device_->getDevice().destroyPipelineLayout(sigmoidPipelineLayout_);
	device_->getDevice().destroyPipelineLayout(weightPipelineLayout_);
}

const vk::PipelineLayout & simpleVK::neuralNetwork::Pipeline::getSigmoidPipelineLayout() const
{
	return sigmoidPipelineLayout_;
}

const vk::PipelineLayout & simpleVK::neuralNetwork::Pipeline::getWeightPipelineLayout() const
{
	return weightPipelineLayout_;
}

size_t simpleVK::neuralNetwork::Pipeline::getSigmoidPipelineCount() const
{
	return sigmoidPipelines_.size();
}

size_t simpleVK::neuralNetwork::Pipeline::getWeightPipelineCount() const
{
	return weightPipelines_.size();
}

const vk::Pipeline & simpleVK::neuralNetwork::Pipeline::getSigmoidPipeline(size_t index) const
{
	return sigmoidPipelines_[index];
}

const vk::Pipeline & simpleVK::neuralNetwork::Pipeline::getWeightPipeline(size_t index) const
{
	return weightPipelines_[index];
}
