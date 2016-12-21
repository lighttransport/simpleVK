#include"shader.h"
#include"device.h"
#include<cstdio>

void simpleVK::neuralNetwork::Shader::readBinary(const std::string & fileName, std::vector<int8_t>& code)
{
	long int size;
	size_t retval;

	std::FILE *fp = std::fopen(fileName.c_str(), "rb");
	if (!fp) return; //TODO implement exception

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);

	code.resize(size);
	retval = std::fread(code.data(), size, 1, fp);

	return; //TODO implement exception
}

void simpleVK::neuralNetwork::Shader::createShaderModule(const std::vector<int8_t>& code, vk::ShaderModule& shaderModule)
{
	//init VertexShaderModuleCreateInfo
	vk::ShaderModuleCreateInfo shaderInfo;
	shaderInfo.setFlags(vk::ShaderModuleCreateFlagBits());
	shaderInfo.setCodeSize(code.size());
	shaderInfo.setPCode(reinterpret_cast<const uint32_t*>(code.data()));
	shaderModule = device_.getDevice().createShaderModule(shaderInfo);
}

simpleVK::neuralNetwork::Shader::Shader(Device& device, std::string fileName) : device_(device)
{
	std::vector<int8_t> code;
	readBinary(fileName, code);
	createShaderModule(code, shaderModule_);
}

simpleVK::neuralNetwork::Shader::~Shader()
{
	device_.getDevice().destroyShaderModule(shaderModule_);
}

const vk::ShaderModule& simpleVK::neuralNetwork::Shader::getShaderModule()
{
	return shaderModule_;
}

