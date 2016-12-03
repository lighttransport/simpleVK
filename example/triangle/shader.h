#pragma once
#include<vulkan/vulkan.hpp>
#include"device.h"

namespace simpleVulkan
{
    class Shader
    {
        vk::Device m_device;
        vk::ShaderModule m_shaderModule;
   public:
        Shader();
        ~Shader();

        Result create(vk::Device device,size_t size,void* code);
        void destroy();

        vk::ShaderModule& getVkShaderModule();
    };
}
