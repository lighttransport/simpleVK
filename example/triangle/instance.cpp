#include"instance.h"

#ifndef VK_API_VERSION
#define VK_API_VERSION VK_API_VERSION_1_0
#endif

namespace simpleVulkan
{
    Instance::Instance(){}
    Instance::~Instance(){}

    Result Instance::create(
            std::string applicationName,
            std::uint32_t applicationVersion,
            std::string engineName,
            std::uint32_t engineVersion,
            const std::vector<const char*>& extensions,
            const std::vector<const char*>& layers)
    {
        vk::Result result;

        //init ApplicationInfo
        vk::ApplicationInfo appInfo;
        appInfo.setPApplicationName(applicationName.c_str()); 
        appInfo.setApplicationVersion(applicationVersion);
        appInfo.setPEngineName(engineName.c_str());
        appInfo.setEngineVersion(engineVersion);
        appInfo.setApiVersion(VK_API_VERSION);
       
        //std::vector<const char*> layers;
        //layers.push_back("VK_LAYER_LUNARG_param_checker");
        //layers.push_back("VK_LAYER_LUNARG_swapchain");
        //layers.push_back("VK_LAYER_LUNARG_mem_tracker");
        //layers.push_back("VK_LAYER_GOOGLE_unique_objects");
        //deviceInfo.enabledLayerCount(layers.size());
        //deviceInfo.ppEnabledLayerNames(layers.data());

        //init InstanceCreateInfo
        vk::InstanceCreateInfo instInfo;
        instInfo.setPApplicationInfo(&appInfo);
        instInfo.setEnabledExtensionCount(extensions.size());
        instInfo.setPpEnabledExtensionNames(extensions.data());
        //instInfo.setEnabledLayerCount(layers.size());
        //instInfo.setPpEnabledLayerNames(layers.data());

        //create Instance
        result = vk::createInstance(&instInfo,nullptr,&m_instance);
      
        return result;
    }

    void Instance::destroy()
    {
        m_instance.destroy(nullptr);
    }

    vk::Instance& Instance::getVkInstance()
    {
        return m_instance;
    }

}
