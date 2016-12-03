#include"descriptor_sets.h"

namespace simpleVulkan
{
    DescriptorSets::DescriptorSets()
    {
    }
    DescriptorSets::~DescriptorSets()
    {
    }

    Result DescriptorSets::create(
            vk::Device device,
            const std::vector<vk::DescriptorSetLayoutBinding>& descriptorBindings,
            uint32_t setsCount)
    {
        vk::Result result;

        m_device = device;

        //init DescriptorSetLayoutCreateInfo
        vk::DescriptorSetLayoutCreateInfo layoutInfo;
        layoutInfo.setBindingCount(descriptorBindings.size());
        layoutInfo.setPBindings(descriptorBindings.data());

        //create DescriptorSetLayout
        result = m_device.createDescriptorSetLayout(&layoutInfo,nullptr,&m_descriptorSetLayout);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }

        //init DescriptroPoolSizes
        std::vector<vk::DescriptorPoolSize> poolSizes(descriptorBindings.size());
        for(size_t i=0;i<poolSizes.size();++i)
        {
            poolSizes[i].setType(descriptorBindings[i].descriptorType);
            poolSizes[i].setDescriptorCount(descriptorBindings[i].descriptorCount);
        }

        //init DescriptorPoolCreateInfo
        vk::DescriptorPoolCreateInfo poolInfo;
        poolInfo.setPoolSizeCount(poolSizes.size());
        poolInfo.setPPoolSizes(poolSizes.data());
        poolInfo.setMaxSets(setsCount);

        //create DescriptorPool
        result = m_device.createDescriptorPool(&poolInfo,nullptr,&m_descriptorPool);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }

        m_descriptorSets.resize(setsCount);

        //init DescriptorSetAllocateInfo
        vk::DescriptorSetAllocateInfo setInfo;
        setInfo.setDescriptorPool(m_descriptorPool);
        setInfo.setDescriptorSetCount(setsCount);
        setInfo.setPSetLayouts(&m_descriptorSetLayout);

        //allocate DescriptorSet
        result = m_device.allocateDescriptorSets(&setInfo,m_descriptorSets.data());
        return result;
    }
    vk::DescriptorSet& DescriptorSets::getVkDescriptorSet(size_t index)
    {
        return m_descriptorSets[index];
    }
    vk::DescriptorSetLayout& DescriptorSets::getVkDescriptorSetLayout()
    {
        return m_descriptorSetLayout;
    }
}
