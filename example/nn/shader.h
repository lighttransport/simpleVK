#pragma once
#include<string>
#include<vector>
#include<vulkan\vulkan.hpp>

namespace simpleVK
{
namespace neuralNetwork
{
class Device;
class Shader
{
private:
	std::shared_ptr<Device> device_;
	vk::ShaderModule shaderModule_;

	void readBinary(const std::string& fileName, std::vector<int8_t>& code);
	void createShaderModule(const std::vector<int8_t>& code, vk::ShaderModule& shaderModule);
public:
	explicit Shader(std::shared_ptr<Device> device, std::string fileName);
	~Shader();

	const vk::ShaderModule& getShaderModule();
};
}
}