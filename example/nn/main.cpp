#include<iostream>
#include<iomanip>
#include"device_manager.h"
#include"resources.h"
#include"shader.h"
#include"pipeline_manager.h"
#include"command_manager.h"


int main()
{
	std::vector<simpleVK::neuralNetwork::LayerSize> layerSizes{
		{4,4},	//input layer
		{2,2},	//output weight layer
		{2,2},	//output sigmoid layer
		{2,2},	//output weight layer
		{2,2}};	//output sigmoid layer
	std::vector<float> input(layerSizes.front().y*layerSizes.front().x);
	std::vector<float> output(layerSizes.back().y*layerSizes.back().x);
	std::vector<float> weight(
		layerSizes[0].y * layerSizes[0].x *
		layerSizes[1].y * layerSizes[1].x);
		std::vector<float> weight2(
		layerSizes[2].y * layerSizes[2].x *
		layerSizes[3].y * layerSizes[3].x);

	for (int i = 0; i < input.size(); ++i)
	{
		input[i] = 1.0f;
	}
	for (int i = 0; i < weight.size(); ++i)
	{
		weight[i] = 0.0625;
	}
	for (int i = 0; i < weight2.size(); ++i)
	{
		weight2[i] = 0.25;
	}

	auto device = std::make_shared<simpleVK::neuralNetwork::DeviceManager>();

	auto resources = std::make_shared<simpleVK::neuralNetwork::Resources>(device, layerSizes);

	auto sigmoidShader = std::make_shared<simpleVK::neuralNetwork::Shader>(device, "sigmoid.spv");
	auto weightShader = std::make_shared<simpleVK::neuralNetwork::Shader>(device, "weight.spv");

	auto pipeline = std::make_shared<simpleVK::neuralNetwork::PipelineManager>(device, resources, sigmoidShader, weightShader);

	resources->writeInputBuffer(input);
	resources->writeWeightBuffer(0, weight);
	resources->writeWeightBuffer(1, weight2);

	auto commandBuffer =
		std::make_shared<simpleVK::neuralNetwork::CommandManager>(device, resources, pipeline);

	commandBuffer->dispach();

	resources->readLayerBuffer(2,output);
	resources->readOutputBuffer(output);

	for (int i = 0; i < output.size(); ++i)
	{
		 std::cout << std::setw(10) << std::left << i << std::right << output[i] << std::endl;
	}
}
