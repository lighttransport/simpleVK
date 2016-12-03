#include<iostream>
#include"image.h"

namespace simpleVulkan
{
    Image::Image()
    {}

    Image::~Image(){}

    Result Image::create(
            vk::Device device,
            vk::Format format,
            vk::ImageUsageFlags usage,
            uint32_t width,
            uint32_t height)
    {
        m_device = device;
        m_format = format;
        m_usage = usage;
        m_width = width;
        m_height = height;

        vk::Result result;

        //init ImageCreateInfo
        vk::ImageCreateInfo imageInfo;
        imageInfo.setImageType(vk::ImageType::e2D);
        imageInfo.setFormat(m_format);
        imageInfo.extent.setWidth(m_width);
        imageInfo.extent.setHeight(m_height);
        imageInfo.extent.setDepth(1);
        imageInfo.setMipLevels(1);
        imageInfo.setArrayLayers(1);
        imageInfo.setSamples(vk::SampleCountFlagBits::e1);
        imageInfo.setTiling(vk::ImageTiling::eLinear);
        imageInfo.setUsage(m_usage);
        imageInfo.setSharingMode(vk::SharingMode::eExclusive);
        imageInfo.setQueueFamilyIndexCount(0);
        imageInfo.setPQueueFamilyIndices(nullptr);
        imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);

        //create DepthImage
        result = m_device.createImage(&imageInfo,nullptr,&m_image); 

        //init MemoryAlloateInfo
        vk::MemoryRequirements req;
        m_device.getImageMemoryRequirements(m_image,&req);
        vk::MemoryAllocateInfo allocInfo;
        allocInfo.setAllocationSize(req.size);
        allocInfo.setMemoryTypeIndex(1); //maybe
        
        //alloc DepthMemory
        result = m_device.allocateMemory(&allocInfo,nullptr,&m_deviceMemory);
    
        //bind Image
        m_device.bindImageMemory(m_image,m_deviceMemory,0);

        vk::ImageAspectFlags aspect;
        if(m_usage & vk::ImageUsageFlagBits::eColorAttachment)
        {
            aspect |= vk::ImageAspectFlagBits::eColor;
        }
        if(m_usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
        {
            aspect |= vk::ImageAspectFlagBits::eDepth;
        }

        //init ImageViewCreateInfo
        vk::ImageViewCreateInfo imageViewInfo;
        imageViewInfo.setFlags(vk::ImageViewCreateFlagBits());
        imageViewInfo.setImage(m_image);
        imageViewInfo.setFormat(m_format);
        imageViewInfo.components.setR(vk::ComponentSwizzle::eR);
        imageViewInfo.components.setG(vk::ComponentSwizzle::eG);
        imageViewInfo.components.setB(vk::ComponentSwizzle::eB);
        imageViewInfo.components.setA(vk::ComponentSwizzle::eA);
        imageViewInfo.subresourceRange.setAspectMask(aspect);
        imageViewInfo.subresourceRange.setBaseMipLevel(0);
        imageViewInfo.subresourceRange.setLevelCount(1);
        imageViewInfo.subresourceRange.setBaseArrayLayer(0);
        imageViewInfo.subresourceRange.setLayerCount(1);
        imageViewInfo.setViewType(vk::ImageViewType::e2D);
       
        //create ImageView
        result = m_device.createImageView(&imageViewInfo,nullptr,&m_imageView);
        return result;
    }

    void Image::destroy()
    {
        m_device.destroyImageView(m_imageView,nullptr);
        m_device.freeMemory(m_deviceMemory,nullptr);
        m_device.destroyImage(m_image,nullptr);
    }

    vk::Image& Image::getVkImage()
    {
        return m_image;
    }
    vk::ImageView& Image::getVkImageView()
    {
        return m_imageView;
    }
    int32_t Image::getWidth()
    {
        return m_width;
    }
    int32_t Image::getHeight()
    {
        return m_height;
    }
}
