#include "resources.h"
#include "devices_manager.h"
#include "window.h"

void simpleVK::multiGPU::Resources::createSwapchain(Swapchain& swapchain)
{
	//get SurfaceFormats
	std::vector<vk::SurfaceFormatKHR> surfaceFormats = devices_->getPhysicalDevice(0).getSurfaceFormatsKHR(window_->getSurface());

	//get SurfacePresentModes
	std::vector<vk::PresentModeKHR> surfacePresentModes = devices_->getPhysicalDevice(0).getSurfacePresentModesKHR(window_->getSurface());

	//get SurfaceCapablilies
	vk::SurfaceCapabilitiesKHR surfaceCapabilities = devices_->getPhysicalDevice(0).getSurfaceCapabilitiesKHR(window_->getSurface());

	if (!(surfaceCapabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eColorAttachment))
	{
		return; //TODO
	}
	if (!(surfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity))
	{
		return; //TODO
	}
	if (!(surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit))
	{
		return; //TODO
	}
	//init SwapchainCreateInfo
	vk::SwapchainCreateInfoKHR swapchainInfo;
	swapchainInfo.setSurface(window_->getSurface());
	swapchainInfo.setMinImageCount(surfaceCapabilities.minImageCount);
	swapchainInfo.setImageFormat(surfaceFormats[0].format);
	swapchainInfo.setImageColorSpace(surfaceFormats[0].colorSpace);
	swapchainInfo.setImageExtent(vk::Extent2D(window_->getWidth(),window_->getHeight()));
	swapchainInfo.setImageArrayLayers(1);
	swapchainInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
	swapchainInfo.setImageSharingMode(vk::SharingMode::eExclusive);
	swapchainInfo.setQueueFamilyIndexCount(0);
	swapchainInfo.setPQueueFamilyIndices(nullptr);
	swapchainInfo.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);
	swapchainInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eInherit);
	swapchainInfo.setPresentMode(surfacePresentModes[0]);
	swapchainInfo.setClipped(true);

	//create Swapchain
	swapchain.swapchain = devices_->getDevice(0).createSwapchainKHR(swapchainInfo);
	swapchain.format = surfaceFormats[0];
}

void simpleVK::multiGPU::Resources::createSwapchainImage(
		const simpleVK::multiGPU::Resources::Swapchain& swapchain,
		simpleVK::multiGPU::Resources::SwapchainImages& images)
{
	images.format = swapchain.format.format;

	//get SwapchainImages
	uint32_t swapchainCount;
	images.images = devices_->getDevice(0).getSwapchainImagesKHR(swapchain.swapchain);

	images.views.resize(images.images.size());

	//create SwapchainImageViews
	for(size_t i=0;i<images.views.size();++i)
	{
		//init SwapchainImageViewCreateInfo
		vk::ImageViewCreateInfo imageViewInfo;
		imageViewInfo.setFlags(vk::ImageViewCreateFlagBits());
		imageViewInfo.setImage(images.images[i]);
		imageViewInfo.setViewType(vk::ImageViewType::e2D);
		imageViewInfo.setFormat(images.format);
		imageViewInfo.components.setR(vk::ComponentSwizzle::eR);
		imageViewInfo.components.setG(vk::ComponentSwizzle::eG);
		imageViewInfo.components.setB(vk::ComponentSwizzle::eB);
		imageViewInfo.components.setA(vk::ComponentSwizzle::eA);
		imageViewInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
		imageViewInfo.subresourceRange.setBaseMipLevel(0);
		imageViewInfo.subresourceRange.setLevelCount(1);
		imageViewInfo.subresourceRange.setBaseArrayLayer(0);
		imageViewInfo.subresourceRange.setLayerCount(1);

		//create SwapchainImageView
		images.views[i] = devices_->getDevice(0).createImageView(imageViewInfo);
	}
}

void simpleVK::multiGPU::Resources::createVertexDescription(
		std::vector<vk::VertexInputBindingDescription>& bindings,
		std::vector<vk::VertexInputAttributeDescription>& attributes)
{
	//init VertexBinding
	bindings.resize(1);
	bindings[0].setBinding(0);
	bindings[0].setInputRate(vk::VertexInputRate::eVertex);
	bindings[0].setStride(sizeof(float) * 5);

	//init VertexAttributes
	attributes.resize(2);
	attributes[0].setBinding(0);
	attributes[0].setLocation(0);
	attributes[0].setFormat(vk::Format::eR32G32B32Sfloat);
	attributes[0].setOffset(0);
	attributes[1].setBinding(0);
	attributes[1].setLocation(1);
	attributes[1].setFormat(vk::Format::eR32G32Sfloat);
	attributes[1].setOffset(sizeof(float) * 3);
}

void simpleVK::multiGPU::Resources::createVertexBuffer(
	size_t gpuIndex,
	vk::DeviceSize size,
	vk::Buffer& buffer)
{
	vk::BufferCreateInfo createInfo;
	createInfo.setSize(size);
	createInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
	buffer = devices_->getDevice(gpuIndex).createBuffer(createInfo);
}

void simpleVK::multiGPU::Resources::createMatrixBuffer(
	size_t gpuIndex,
	vk::DeviceSize size,
	vk::Buffer& buffer)
{
	vk::BufferCreateInfo createInfo;
	createInfo.setSize(size);
	createInfo.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);
	buffer = devices_->getDevice(gpuIndex).createBuffer(createInfo);
}

void simpleVK::multiGPU::Resources::createResultImage(size_t gpuIndex, vk::Image& image)
{
	//init ImageCreateInfo
	vk::ImageCreateInfo imageInfo;
	imageInfo.setImageType(vk::ImageType::e2D);
	imageInfo.setFormat(swapchainImages_.format);
	imageInfo.extent.setWidth(window_->getWidth()/2);
	imageInfo.extent.setHeight(window_->getHeight());
	imageInfo.extent.setDepth(1);
	imageInfo.setMipLevels(1);
	imageInfo.setArrayLayers(1);
	imageInfo.setSamples(vk::SampleCountFlagBits::e1);
	imageInfo.setTiling(vk::ImageTiling::eLinear);
	imageInfo.setUsage(vk::ImageUsageFlagBits::eColorAttachment);
	imageInfo.setSharingMode(vk::SharingMode::eExclusive);
	imageInfo.setQueueFamilyIndexCount(0);
	imageInfo.setPQueueFamilyIndices(nullptr);
	imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
	image = devices_->getDevice(gpuIndex).createImage(imageInfo);
}

void simpleVK::multiGPU::Resources::createAndBindMemory(
	size_t gpuIndex,
	const vk::Buffer & buffer,
	vk::DeviceMemory & memory)
{
	//get PhysicalDeviceMemoryProperties
	vk::PhysicalDeviceMemoryProperties memoryProperties;
	devices_->getPhysicalDevice(gpuIndex).getMemoryProperties(&memoryProperties);

	//get MemoryRequirements
	vk::MemoryRequirements req;
	devices_->getDevice(gpuIndex).getBufferMemoryRequirements(buffer, &req);

	uint32_t memoryTypeBits = req.memoryTypeBits;
	uint32_t memoryTypeIndex = 0;
	for (int i = 0; i < (sizeof(memoryTypeBits) * 8); ++i)
	{
		if ((memoryTypeBits >> i) & 1)
		{
			if (memoryProperties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible)
			{
				memoryTypeIndex = i;
				break;
			}
		}
	}

	//init MemoryAllocateInfo
	vk::MemoryAllocateInfo memoryInfo;
	memoryInfo.setMemoryTypeIndex(memoryTypeIndex);
	memoryInfo.setAllocationSize(req.size);

	//allocate VertexMemory
	memory = devices_->getDevice(gpuIndex).allocateMemory(memoryInfo);
	//bind VertexMemory to VertexBuffer
	devices_->getDevice(gpuIndex).bindBufferMemory(buffer, memory, 0);
}

void simpleVK::multiGPU::Resources::createAndBindMemory(
	size_t gpuIndex,
	const vk::Image & image,
	vk::DeviceMemory & memory)
{
	//init MemoryAlloateInfo
	vk::MemoryRequirements req;
	devices_->getDevice(0).getImageMemoryRequirements(image,&req);
	vk::MemoryAllocateInfo allocInfo;
	allocInfo.setAllocationSize(req.size);
	allocInfo.setMemoryTypeIndex(1); //maybe

	//alloc DepthMemory
	memory = devices_->getDevice(gpuIndex).allocateMemory(allocInfo);

	//bind Image
	devices_->getDevice(gpuIndex).bindImageMemory(image,memory,0);
}

void simpleVK::multiGPU::Resources::createLeftDescriptorPool(vk::DescriptorPool& pool)
{
	std::vector<vk::DescriptorPoolSize> poolSizes(2);
	poolSizes[0].setType(vk::DescriptorType::eUniformBuffer);
	poolSizes[0].setDescriptorCount(1); //MatrixBuffer*1
	poolSizes[1].setType(vk::DescriptorType::eUniformBuffer);
	poolSizes[1].setDescriptorCount(2); //ResultImage*2

	vk::DescriptorPoolCreateInfo createInfo;
	createInfo.setMaxSets(2); //MatrixDescriptor, CompositeDescriptor
	createInfo.setPPoolSizes(poolSizes.data());
	createInfo.setPoolSizeCount(static_cast<uint32_t>(poolSizes.size()));
	createInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

	pool = devices_->getDevice(0).createDescriptorPool(createInfo);
}

void simpleVK::multiGPU::Resources::createRightDescriptorPool(vk::DescriptorPool& pool)
{
	std::vector<vk::DescriptorPoolSize> poolSizes(1);
	poolSizes[0].setType(vk::DescriptorType::eUniformBuffer);
	poolSizes[0].setDescriptorCount(1); //MatrixBuffer*1

	vk::DescriptorPoolCreateInfo createInfo;
	createInfo.setMaxSets(1); //MatrixDescriptor
	createInfo.setPPoolSizes(poolSizes.data());
	createInfo.setPoolSizeCount(static_cast<uint32_t>(poolSizes.size()));
	createInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

	pool = devices_->getDevice(1).createDescriptorPool(createInfo);
}

void simpleVK::multiGPU::Resources::createMatrixDescriptorSetLayout(
	size_t gpuIndex,
	vk::DescriptorSetLayout & setLayout)
{
	std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings(1);
	setLayoutBindings[0].setBinding(0);
	setLayoutBindings[0].setDescriptorType(vk::DescriptorType::eUniformBuffer);
	setLayoutBindings[0].setDescriptorCount(1);
	setLayoutBindings[0].setStageFlags(vk::ShaderStageFlagBits::eVertex);

	vk::DescriptorSetLayoutCreateInfo createInfo;
	createInfo.setBindingCount(static_cast<uint32_t>(setLayoutBindings.size()));
	createInfo.setPBindings(setLayoutBindings.data());

	setLayout = devices_->getDevice(gpuIndex).createDescriptorSetLayout(createInfo);
}

void simpleVK::multiGPU::Resources::createMatrixDescriptorSet(
		size_t gpuIndex,
		const vk::DescriptorSetLayout& layout,
		const vk::DescriptorPool& pool,
		vk::DescriptorSet& set)
{
	vk::DescriptorSetAllocateInfo allocateInfo;
	allocateInfo.setPSetLayouts(&layout);
	allocateInfo.setDescriptorSetCount(1);
	allocateInfo.setDescriptorPool(pool);
	set = devices_->getDevice(gpuIndex).allocateDescriptorSets(allocateInfo)[0];
}

void simpleVK::multiGPU::Resources::createCompositeDescriptorSetLayout(vk::DescriptorSetLayout & setLayout)
{
	std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings(2);
	setLayoutBindings[0].setBinding(0);
	setLayoutBindings[0].setDescriptorType(vk::DescriptorType::eUniformBuffer);
	setLayoutBindings[0].setDescriptorCount(1);
	setLayoutBindings[0].setStageFlags(vk::ShaderStageFlagBits::eFragment);
	setLayoutBindings[1].setBinding(1);
	setLayoutBindings[1].setDescriptorType(vk::DescriptorType::eUniformBuffer);
	setLayoutBindings[1].setDescriptorCount(1);
	setLayoutBindings[1].setStageFlags(vk::ShaderStageFlagBits::eFragment);

	vk::DescriptorSetLayoutCreateInfo createInfo;
	createInfo.setBindingCount(static_cast<uint32_t>(setLayoutBindings.size()));
	createInfo.setPBindings(setLayoutBindings.data());

	setLayout = devices_->getDevice(0).createDescriptorSetLayout(createInfo);
}

void simpleVK::multiGPU::Resources::createComositeDescriptorSet(
		const vk::DescriptorSetLayout& layout,
		const vk::DescriptorPool& pool,
		vk::DescriptorSet& set)
{
	vk::DescriptorSetAllocateInfo allocateInfo;
	allocateInfo.setPSetLayouts(&layout);
	allocateInfo.setDescriptorSetCount(1);
	allocateInfo.setDescriptorPool(pool);
	set = devices_->getDevice(0).allocateDescriptorSets(allocateInfo)[0];
}

simpleVK::multiGPU::Resources::Resources(
	std::shared_ptr<DevicesManager> devices, 
	std::shared_ptr<Window> window):
	devices_(devices),
	window_(window)
{
	//Swapchain setting
	createSwapchain(swapchain_);
	createSwapchainImage(swapchain_,swapchainImages_);

	//Mesh setting 
	createVertexDescription(leftMesh_.bindings,leftMesh_.attributes);
	createVertexDescription(rightMesh_.bindings,rightMesh_.attributes);
	createVertexDescription(compositedMesh_.bindings,compositedMesh_.attributes);

	createVertexBuffer(0,sizeof(float)*5*3,leftMesh_.buffer);
	createVertexBuffer(1,sizeof(float)*5*3,rightMesh_.buffer);
	createVertexBuffer(0,sizeof(float)*5*6,compositedMesh_.buffer);

	createAndBindMemory(0,leftMesh_.buffer,leftMesh_.memory);
	createAndBindMemory(1,rightMesh_.buffer,rightMesh_.memory);
	createAndBindMemory(0,compositedMesh_.buffer,compositedMesh_.memory);

	//Matrix setting
	createMatrixBuffer(0,sizeof(float)*3*3,leftMatrix_.buffer);
	createMatrixBuffer(1,sizeof(float)*3*3,rightMatrix_.buffer);

	createAndBindMemory(0,leftMatrix_.buffer,leftMatrix_.memory);
	createAndBindMemory(1,rightMatrix_.buffer,rightMatrix_.memory);

	//Image setting
	createResultImage(0,results_.left.image);
	createResultImage(0,results_.right.image);
	createResultImage(1,tmpResult_.image);
	
	//createAndBindMemory TODO
	createAndBindMemory(0,results_.left.image,results_.left.memory);
	createAndBindMemory(0,results_.right.image,results_.right.memory);
	createAndBindMemory(1,tmpResult_.image,tmpResult_.memory);

	createLeftDescriptorPool(leftPool_);
	createRightDescriptorPool(rightPool_);

	createMatrixDescriptorSetLayout(0,leftMatrix_.layout);
	createMatrixDescriptorSetLayout(1,rightMatrix_.layout);
	createCompositeDescriptorSetLayout(results_.layout);

	createMatrixDescriptorSet(
			0,
			leftMatrix_.layout,
			leftPool_,
			leftMatrix_.set);
	createMatrixDescriptorSet(
			1,
			rightMatrix_.layout,
			rightPool_,
			rightMatrix_.set);
	createComositeDescriptorSet(
			results_.layout,
			leftPool_,
			results_.set);
}

size_t simpleVK::multiGPU::Resources::getSwapchainCount() const
{
	return swapchainImages_.images.size();
}

const vk::Image& simpleVK::multiGPU::Resources::getSwapchainImage(size_t index) const
{
	return swapchainImages_.images[index];
}
