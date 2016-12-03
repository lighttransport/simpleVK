#include "pipeline.h"

#include<iostream>

#include"utility.h"
#include"device.h"
#include"resources.h"

void simpleVK::Pipeline::createPipelineCache(vk::PipelineCache & pipelineCache)
{
        //init PipelineCacheCreateInfo
        vk::PipelineCacheCreateInfo cacheInfo;

        //create PipelineCache
		pipelineCache = device_.getDevice().createPipelineCache(cacheInfo);
}

void simpleVK::Pipeline::createPipelineLayout(vk::PipelineLayout & pipelineLayout)
{
        //init PipelineLayoutCreateInfo
        vk::PipelineLayoutCreateInfo layoutInfo;
        layoutInfo.setPushConstantRangeCount(0);
        layoutInfo.setPPushConstantRanges(nullptr);
        layoutInfo.setSetLayoutCount(1);
        layoutInfo.setPSetLayouts(&(resources_.getDescriptorSetLayout()));

		//create PipelineLayout
		pipelineLayout = device_.getDevice().createPipelineLayout(layoutInfo);
}

void simpleVK::Pipeline::createShader(vk::ShaderModule & shader)
{
	//load ShaderCode
	std::vector<uint8_t> code;
	readBinaryFile("comp.spv", code);

	//init VertexShaderModuleCreateInfo
	vk::ShaderModuleCreateInfo shaderInfo;
	shaderInfo.setFlags(vk::ShaderModuleCreateFlagBits());
	shaderInfo.setCodeSize(code.size());
	shaderInfo.setPCode(reinterpret_cast<uint32_t*>(code.data()));

	//create VertexShader
	shader = device_.getDevice().createShaderModule(shaderInfo);
}

void simpleVK::Pipeline::createPipeline(const vk::PipelineCache & pipelineCache, const vk::PipelineLayout & pipelineLayout, const vk::ShaderModule & shader, vk::Pipeline & pipeline)
{
  //init PipelineShaderStageCreateInfo
  vk::PipelineShaderStageCreateInfo stageInfo;
  stageInfo.setStage(vk::ShaderStageFlagBits::eCompute);
  stageInfo.setModule(shader);
  stageInfo.setPName("main");
  stageInfo.setPSpecializationInfo(nullptr);
  
  //init ComputePipelineCreateInfo
  vk::ComputePipelineCreateInfo createInfo;
  createInfo.setStage(stageInfo);
  createInfo.setLayout(pipelineLayout);
  
  //create ComputePipeline
  pipeline = device_.getDevice().createComputePipeline(pipelineCache,createInfo);
}

simpleVK::Pipeline::Pipeline(Device & device,Resources & resources):
	device_(device),
	resources_(resources)
{
	createPipelineCache(pipelineCache_);
	createPipelineLayout(pipelineLayout_);
	createShader(shaderModule_);
	createPipeline(pipelineCache_, pipelineLayout_, shaderModule_, pipeline_);
}

simpleVK::Pipeline::~Pipeline()
{
	device_.getDevice().destroyPipeline(pipeline_);
	device_.getDevice().destroyShaderModule(shaderModule_);
	device_.getDevice().destroyPipelineLayout(pipelineLayout_);
	device_.getDevice().destroyPipelineCache(pipelineCache_);
}

const vk::PipelineLayout & simpleVK::Pipeline::getPipelineLayout() const
{
	return pipelineLayout_;
}

const vk::Pipeline & simpleVK::Pipeline::getPipeline() const
{
	return pipeline_;
}
