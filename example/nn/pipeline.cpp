#include "pipeline.h"

#include<iostream>

#include"utility.h"
#include"device.h"
#include"resources.h"
#include"shader.h"

void simpleVK::neuralNetwork::Pipeline::createPipelineCache(vk::PipelineCache & pipelineCache)
{
        //init PipelineCacheCreateInfo
        vk::PipelineCacheCreateInfo cacheInfo;

        //create PipelineCache
		pipelineCache = device_.getDevice().createPipelineCache(cacheInfo);
}

void simpleVK::neuralNetwork::Pipeline::createPipelineLayout(vk::PipelineLayout & pipelineLayout)
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

void simpleVK::neuralNetwork::Pipeline::createPipeline(const vk::PipelineCache & pipelineCache, const vk::PipelineLayout & pipelineLayout, const vk::ShaderModule & shader, vk::Pipeline & pipeline)
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

simpleVK::neuralNetwork::Pipeline::Pipeline(Device & device,Resources & resources,Shader & shader):
	device_(device),
	resources_(resources),
	shader_(shader)
{
	createPipelineCache(pipelineCache_);
	createPipelineLayout(pipelineLayout_);
	createPipeline(pipelineCache_, pipelineLayout_,shader_.getShaderModule(), pipeline_);
}

simpleVK::neuralNetwork::Pipeline::~Pipeline()
{
	device_.getDevice().destroyPipeline(pipeline_);
	device_.getDevice().destroyPipelineLayout(pipelineLayout_);
	device_.getDevice().destroyPipelineCache(pipelineCache_);
}

const vk::PipelineLayout & simpleVK::neuralNetwork::Pipeline::getPipelineLayout() const
{
	return pipelineLayout_;
}

const vk::Pipeline & simpleVK::neuralNetwork::Pipeline::getPipeline() const
{
	return pipeline_;
}
