#pragma once
#include<vulkan\vulkan.hpp>
#include<string>
#include<vector>

namespace simpleVK
{
	class Device;
	class Shader
	{
	private:
		Device& device_;
		vk::ShaderModule shaderModule_;

		void readBinary(const std::string& fileName,std::vector<int8_t>& code);
		void createShaderModule(const std::vector<int8_t>& code,vk::ShaderModule& shaderModule);
	public:
		explicit Shader(Device& device,std::string fileName);
		~Shader();

		const vk::ShaderModule& getShaderModule();
	};
}
