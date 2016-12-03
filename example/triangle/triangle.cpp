#include<cmath>
#include<array>
#include<vector>
#include<iostream>
#include<string>
#include<vulkan/vulkan.hpp>
#include<vulkan/vulkan.h>
#include<GLFW/glfw3.h>
#include"simple_vulkan.h"

#define ENABLE_VALIDATION	(1)

#if ENABLE_VALIDATION
VKAPI_ATTR VkBool32 VKAPI_CALL MyDebugReportCallback(
    VkDebugReportFlagsEXT       flags,
    VkDebugReportObjectTypeEXT  objectType,
    uint64_t                    object,
    size_t                      location,
    int32_t                     messageCode,
    const char*                 pLayerPrefix,
    const char*                 pMessage,
    void*                       pUserData)
{
    std::cerr << pMessage << std::endl;
    return VK_FALSE;
}
#endif

class TestApplication : public simpleVulkan::Application
{
	vk::Format m_colorFormat;
	vk::Format m_depthFormat = vk::Format::eD24UnormS8Uint;

	float m_vertexes[3][2];
	float m_matrix[2][4];

	//= {
	//    {1.0f,0.0f,0.0f,0.0f},
	//    {0.0f,1.0f,0.0f,0.0f}
	//};

	const std::string m_vertexShaderName = "./vert.spv";
	const std::string m_fragShaderName = "./frag.spv";

	simpleVulkan::Instance* m_instance;
	simpleVulkan::Device* m_device;
	simpleVulkan::Queue* m_queue;
	simpleVulkan::CommandBuffers* m_cmdBuf;
	simpleVulkan::Swapchain* m_swapchain;
	simpleVulkan::Image* m_depthImage;
	simpleVulkan::RenderPass* m_renderPass;
	std::vector<simpleVulkan::Framebuffer*> m_FrameBuffers;
	simpleVulkan::Buffer* m_vertexBuffer;
	simpleVulkan::Buffer* m_matrixBuffer;
	simpleVulkan::DescriptorSets* m_descriptorSets;
	simpleVulkan::Shader* m_vertexShader;
	simpleVulkan::Shader* m_fragmentShader;
	simpleVulkan::GraphicsPipeline* m_pipeline;

    VkSemaphore a;
	vk::SurfaceKHR m_windowSurface;
	vk::Semaphore m_semaphore;
	vk::Viewport m_viewport;
	vk::Rect2D m_scissor;


	uint32_t m_bufferIndex;
	uint32_t m_count;

public:
	using simpleVulkan::Application::Application;
private:

	virtual bool initialize(
		const std::vector<const char*>& glfwExtensions,
		GLFWwindow* window) override
	{

		m_vertexes[0][0] =  0.0f;
		m_vertexes[0][1] =  1.0f;
		m_vertexes[1][0] =  0.86f;
		m_vertexes[1][1] = -0.5f;
		m_vertexes[2][0] = -0.86f;
		m_vertexes[2][1] = -0.5f;


		setInterval(std::chrono::milliseconds(10));
		vk::Result result;

		std::vector<const char*> layers;

		if (getValidateFlag())
		{
			layers.push_back("VK_LAYER_LUNARG_standard_validation");
		}

		m_instance = new simpleVulkan::Instance();
		vk::Result ret = m_instance->create("testApp", 1, "testEngine", 1, glfwExtensions, layers);
		if (ret != vk::Result::eSuccess) {
			std::cerr << "failed to create instance!" << std::endl;
			exit(-1);
		}

		if (getValidateFlag())
		{
			vk::DebugReportCallbackCreateInfoEXT callbackInfo(
				vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eDebug,
				MyDebugReportCallback, nullptr);

			// @todo { acquire proc address somewhewre, not here. }
			{
				VkInstance instance = (m_instance->getVkInstance());
				/* Load VK_EXT_debug_report entry points in debug builds */
				PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT =
					reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>
						(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
				PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT =
					reinterpret_cast<PFN_vkDebugReportMessageEXT>
						(vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT"));
				PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT =
					reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>
						(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));

					/* Setup callback creation information */
				VkDebugReportCallbackCreateInfoEXT callbackCreateInfo;
				callbackCreateInfo.sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
				callbackCreateInfo.pNext       = nullptr;
				callbackCreateInfo.flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT |
												 VK_DEBUG_REPORT_WARNING_BIT_EXT |
												 VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
				callbackCreateInfo.pfnCallback = &MyDebugReportCallback;
				callbackCreateInfo.pUserData   = nullptr;

				/* Register the callback */
				VkDebugReportCallbackEXT callback;
				VkResult result = vkCreateDebugReportCallbackEXT(instance, &callbackCreateInfo, nullptr, &callback);
				if (result != VK_SUCCESS) {
					std::cerr << "failed to create debug report callback!" << std::endl;
					exit(-1);
				}
			}
		}

		m_device = new simpleVulkan::Device();
		m_device->create(m_instance->getVkInstance(), getValidateFlag());

		m_queue = new simpleVulkan::Queue();
		m_queue->init(m_device->getVkDevice(0));

		m_cmdBuf = new simpleVulkan::CommandBuffers();
		m_cmdBuf->create(m_device->getVkDevice(0), 2);
		{
			//init CommandBufferInheritanceInfo
			vk::CommandBufferInheritanceInfo cmdBufInheritanceInfo;

			//init CommandBufferBeginInfo
			vk::CommandBufferBeginInfo cmdBufBeginInfo;
			cmdBufBeginInfo.setPInheritanceInfo(&cmdBufInheritanceInfo);

			//begin CommandBuffer
			result = m_cmdBuf->getVkCommandBuffer(0).begin(&cmdBufBeginInfo);
		}

		//get WindowSurface
		glfwCreateWindowSurface(static_cast<VkInstance>(m_instance->getVkInstance()), window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&m_windowSurface));

		m_swapchain = new simpleVulkan::Swapchain();
		m_swapchain->create(m_device->getVkPhysicalDevice(0), m_device->getVkDevice(0), m_windowSurface, vk::ImageUsageFlagBits::eColorAttachment, getWidth(), getHeight());
		m_colorFormat = m_swapchain->getFormat();

		for (int i = 0; i < m_swapchain->count(); ++i)
		{
			//init ImageMemoryBarrier
			vk::ImageMemoryBarrier barrier;
			barrier.setSrcAccessMask(vk::AccessFlagBits());
			barrier.setDstAccessMask(vk::AccessFlagBits::eMemoryRead);
			barrier.setOldLayout(vk::ImageLayout::eUndefined);
			barrier.setNewLayout(vk::ImageLayout::ePresentSrcKHR);
			barrier.setImage(m_swapchain->getVkImage(i));
			barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
			barrier.subresourceRange.setBaseMipLevel(0);
			barrier.subresourceRange.setLevelCount(1);
			barrier.subresourceRange.setBaseArrayLayer(0);
			barrier.subresourceRange.setLayerCount(1);

			//layout SwapchainImage
			m_cmdBuf->getVkCommandBuffer(0).pipelineBarrier(
			        vk::PipelineStageFlagBits::eTopOfPipe,
			        vk::PipelineStageFlagBits::eTopOfPipe,
			        vk::DependencyFlagBits(),
			        0,
			        nullptr,
			        0,
			        nullptr,
		  	        1,
			        &barrier);
		}

		m_depthImage = new simpleVulkan::Image();
		m_depthImage->create(
			m_device->getVkDevice(0),
			m_depthFormat,
			vk::ImageUsageFlagBits::eDepthStencilAttachment,
			getWidth(),
			getHeight());

		//init ImageMemoryBarrier
		vk::ImageMemoryBarrier barrier;
		barrier.setSrcAccessMask(vk::AccessFlagBits());
		barrier.setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead);
		barrier.setOldLayout(vk::ImageLayout::eUndefined);
		barrier.setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
		barrier.setImage(m_depthImage->getVkImage());
		barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth);
		barrier.subresourceRange.setBaseMipLevel(0);
		barrier.subresourceRange.setLevelCount(1);
		barrier.subresourceRange.setBaseArrayLayer(0);
		barrier.subresourceRange.setLayerCount(1);

		//layout DepthImage
		//m_cmdBuf->getVkCommandBuffer(0).pipelineBarrier(
		//		vk::PipelineStageFlagBits::eTopOfPipe,
		//		vk::PipelineStageFlagBits::eTopOfPipe,
		//		vk::DependencyFlagBits(),
		//		0,
		//		nullptr,
		//		0,
		//		nullptr,
		//		1,
		//		&barrier);

		m_renderPass = new simpleVulkan::RenderPass();
		m_renderPass->create(m_device->getVkDevice(0), m_colorFormat, m_depthFormat);

		{
			m_FrameBuffers.resize(m_swapchain->count());
			for (size_t i = 0; i < m_FrameBuffers.size(); ++i)
			{
				m_FrameBuffers[i] = new simpleVulkan::Framebuffer();
				m_FrameBuffers[i]->create(
					m_device->getVkDevice(0),
					getWidth(),
					getHeight(),
					m_swapchain->getVkImageView(i),
					m_depthImage->getVkImageView(),
					m_renderPass->getVkRenderPass());
			}
		}

		//end CommandBuffer
		m_cmdBuf->getVkCommandBuffer(0).end();
		//init SubmitInfo
		m_queue->submit(m_cmdBuf->getVkCommandBuffer(0));

		//wait queue
		m_queue->wait();

		//read VertexShader
		std::vector<uint8_t> code(0);
		if (!readBinaryFile(m_vertexShaderName, code))
		{
			std::cout << "could not read vertex shader!!" << std::endl;
		}

		m_vertexShader = new simpleVulkan::Shader();
		m_vertexShader->create(m_device->getVkDevice(0), code.size(), code.data());

		//read FragShader
		code.clear();
		if (!readBinaryFile(m_fragShaderName, code))
		{
			std::cout << "could not read fragment shader!!" << std::endl;
		}

		m_fragmentShader = new simpleVulkan::Shader();
		m_fragmentShader->create(m_device->getVkDevice(0), code.size(), code.data());

		std::vector<vk::VertexInputBindingDescription> vertexBindings(1);
		std::vector<vk::VertexInputAttributeDescription> vertexAttributes(1);

		//init VertexBinding
		vertexBindings[0].setBinding(0);
		vertexBindings[0].setInputRate(vk::VertexInputRate::eVertex);
		vertexBindings[0].setStride(sizeof(float) * 2);

		//init VertexAttributes
		vertexAttributes[0].setBinding(0);
		vertexAttributes[0].setLocation(0);
		vertexAttributes[0].setFormat(vk::Format::eR32G32Sfloat);
		vertexAttributes[0].setOffset(0);

		//create VertexBuffer
		m_vertexBuffer = new simpleVulkan::Buffer();
		m_vertexBuffer->create(m_device->getVkPhysicalDevice(0), m_device->getVkDevice(0), vk::BufferUsageFlagBits::eVertexBuffer, sizeof(m_vertexes));

		//write VertexBuffer
		m_vertexBuffer->write(reinterpret_cast<const void*>(m_vertexes));

		//create FragmentBuffer
		m_matrixBuffer = new simpleVulkan::Buffer();
		m_matrixBuffer->create(m_device->getVkPhysicalDevice(0), m_device->getVkDevice(0), vk::BufferUsageFlagBits::eUniformBuffer, sizeof(m_matrix));

		//write FragmentBuffer
		m_matrixBuffer->write(reinterpret_cast<const void*>(m_matrix));

		//init DescriptorSetLayoutBinding
		std::vector<vk::DescriptorSetLayoutBinding> layoutBinding(1);
		layoutBinding[0].setBinding(0);
		layoutBinding[0].setDescriptorType(vk::DescriptorType::eUniformBuffer);
		layoutBinding[0].setStageFlags(vk::ShaderStageFlagBits::eVertex);
		layoutBinding[0].setPImmutableSamplers(nullptr);
		layoutBinding[0].setDescriptorCount(1);

		m_descriptorSets = new simpleVulkan::DescriptorSets();
		m_descriptorSets->create(m_device->getVkDevice(0), layoutBinding, 1);

		//write DescriptorSet
		{

			//init DescriptorBufferInfo
			vk::DescriptorBufferInfo bufferInfo;
			bufferInfo.setBuffer(m_matrixBuffer->getVkBuffer());
			bufferInfo.setOffset(0);
			bufferInfo.setRange(sizeof(m_matrix));

			//init WriteDescriptorSet
			vk::WriteDescriptorSet writeSet;
			writeSet.setDstSet(m_descriptorSets->getVkDescriptorSet(0));
			writeSet.setDstBinding(0);
			writeSet.setDstArrayElement(0);
			writeSet.setDescriptorCount(1);
			writeSet.setDescriptorType(vk::DescriptorType::eUniformBuffer);
			writeSet.setPBufferInfo(&bufferInfo);

			//update DescriptorSet
			m_device->getVkDevice(0).updateDescriptorSets(1, &writeSet, 0, nullptr);
		}

		//init Viewport
		m_viewport.setX(0.0f);
		m_viewport.setY(0.0f);
		m_viewport.setWidth(getWidth());
		m_viewport.setHeight(getHeight());
		m_viewport.setMinDepth(0.0f);
		m_viewport.setMaxDepth(1.0f);

		//init Scissor
		m_scissor.offset.setX(0);
		m_scissor.offset.setY(0);
		m_scissor.extent.setWidth(getWidth());
		m_scissor.extent.setHeight(getHeight());

		m_pipeline = new simpleVulkan::GraphicsPipeline();
		m_pipeline->create(
			m_device->getVkDevice(0),
			m_vertexShader->getVkShaderModule(),
			m_fragmentShader->getVkShaderModule(),
			m_descriptorSets->getVkDescriptorSetLayout(),
			vertexBindings,
			vertexAttributes,
			m_viewport,
			m_scissor,
			m_renderPass->getVkRenderPass());
		{
			//init SemaphoreCreateInfo
			vk::SemaphoreCreateInfo semaphoreInfo;
			semaphoreInfo.setFlags(vk::SemaphoreCreateFlagBits());

			//create Semaphore
			result = m_device->getVkDevice(0).createSemaphore(
                    &semaphoreInfo,
                    nullptr,
                    &m_semaphore);
		}

		return true;

	}
	virtual void finalize() override
	{
	}

	virtual bool render() override
	{
		vk::Result result;
		float rate = 100 / (3.14f * 2.0f);
		m_matrix[0][0] = std::cos(m_count / rate);
		m_matrix[0][1] = std::sin(m_count / rate);
		m_matrix[1][0] = -std::sin(m_count / rate);
		m_matrix[1][1] = std::cos(m_count / rate);
		m_matrixBuffer->write(reinterpret_cast<void*>(&m_matrix));

		//acquire next Image
		result = m_device->getVkDevice(0).acquireNextImageKHR(
                m_swapchain->getVkSwapchainKHR(),
                400000000, 
                m_semaphore,
                nullptr,
                &m_bufferIndex);

		{
			//init CommandBufferInheritanceInfo
			vk::CommandBufferInheritanceInfo inheritanceInfo;
			inheritanceInfo.setRenderPass(m_renderPass->getVkRenderPass()); //debug
			inheritanceInfo.setSubpass(0); //debug
			inheritanceInfo.setFramebuffer(m_FrameBuffers[m_bufferIndex]->getVkFrameBuffer()); //debug
			inheritanceInfo.setOcclusionQueryEnable(false);
			inheritanceInfo.setQueryFlags(vk::QueryControlFlagBits());
			inheritanceInfo.setPipelineStatistics(vk::QueryPipelineStatisticFlagBits());

			//init CommandBufferBeginInfo
			vk::CommandBufferBeginInfo beginInfo;
			beginInfo.setPInheritanceInfo(&inheritanceInfo);

			//begin CommandBuffer
			m_cmdBuf->getVkCommandBuffer(1).begin(&beginInfo);

			{
				//pipelineBarrier
				vk::ImageMemoryBarrier barrier;
				barrier.setSrcAccessMask(vk::AccessFlagBits::eMemoryRead);
				barrier.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
				barrier.setOldLayout(vk::ImageLayout::ePresentSrcKHR);
				barrier.setNewLayout(vk::ImageLayout::eColorAttachmentOptimal);
				barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
				barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
				barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
				barrier.subresourceRange.setBaseMipLevel(0);
				barrier.subresourceRange.setLayerCount(1);
				barrier.subresourceRange.setBaseArrayLayer(0);
				barrier.subresourceRange.setLevelCount(1);
				barrier.setImage(m_swapchain->getVkImage(m_bufferIndex));
				m_cmdBuf->getVkCommandBuffer(1).pipelineBarrier(
					vk::PipelineStageFlagBits::eAllCommands,
					vk::PipelineStageFlagBits::eTopOfPipe,
					vk::DependencyFlagBits(),
					0,
					nullptr,
					0,
					nullptr,
					1,
					&barrier);
			}

		}

		{
			//clear color
			vk::ClearColorValue clearColor(std::array<float, 4>{0.0f, 1.0f, 0.0f, 1.0f});
			vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
			m_cmdBuf->getVkCommandBuffer(1).clearColorImage(m_swapchain->getVkImage(m_bufferIndex), vk::ImageLayout::eGeneral, &clearColor, 1, &range);
		}

		{
			//clear depth
			vk::ClearDepthStencilValue clearDepth(1.0f, 0);
			vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1);
			m_cmdBuf->getVkCommandBuffer(1).clearDepthStencilImage(m_depthImage->getVkImage(), vk::ImageLayout::eGeneral, &clearDepth, 1, &range);
		}

		{
			//clear image
			vk::ClearColorValue clearColor(std::array<float, 4>{1.0f, 0.0f, 0.0f, 1.0f});
			vk::ClearDepthStencilValue clearDepth(1.0f, 0.0f);
			vk::ClearValue clearValues[2];
			clearValues[0].setColor(clearColor);
			clearValues[1].setDepthStencil(clearDepth);

			//init RenderPassBeginInfo
			vk::RenderPassBeginInfo passInfo;
			passInfo.setRenderPass(m_renderPass->getVkRenderPass());
			passInfo.setFramebuffer(m_FrameBuffers[m_bufferIndex]->getVkFrameBuffer());
			passInfo.renderArea.offset.setX(0);
			passInfo.renderArea.offset.setY(0);
			passInfo.renderArea.extent.setWidth(getWidth());
			passInfo.renderArea.extent.setHeight(getHeight());
			passInfo.setClearValueCount(2);
			passInfo.setPClearValues(clearValues);

			//begin RenderPass
			m_cmdBuf->getVkCommandBuffer(1).beginRenderPass(&passInfo, vk::SubpassContents::eInline);
		}

		//bind Pipeline
		m_cmdBuf->getVkCommandBuffer(1).bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline->getVkPipeline());

		//bind DescriptorSet
		m_cmdBuf->getVkCommandBuffer(1).bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipeline->getVkPipelineLayout(), 0, 1, &m_descriptorSets->getVkDescriptorSet(0), 0, nullptr);

		//set Viewport
		m_cmdBuf->getVkCommandBuffer(1).setViewport(0, 1, &m_viewport);

		//set Scissor
		m_cmdBuf->getVkCommandBuffer(1).setScissor(0, 1, &m_scissor);

		//bind VertexBuffer
		vk::DeviceSize offset(0);
		m_cmdBuf->getVkCommandBuffer(1).bindVertexBuffers(0, 1, &m_vertexBuffer->getVkBuffer(), &offset);

		//draw
		m_cmdBuf->getVkCommandBuffer(1).draw(3, 1, 0, 0);

		//end RenderPass
		m_cmdBuf->getVkCommandBuffer(1).endRenderPass();

		{
			//pipelineBarrier
			vk::ImageMemoryBarrier barrier;
			barrier.setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
			barrier.setDstAccessMask(vk::AccessFlagBits::eMemoryRead);
			barrier.setOldLayout(vk::ImageLayout::eColorAttachmentOptimal);
			barrier.setNewLayout(vk::ImageLayout::ePresentSrcKHR);
			barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
			barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
			barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
			barrier.subresourceRange.setBaseMipLevel(0);
			barrier.subresourceRange.setLayerCount(1);
			barrier.subresourceRange.setBaseArrayLayer(0);
			barrier.subresourceRange.setLevelCount(1);
			barrier.setImage(m_swapchain->getVkImage(m_bufferIndex));
			m_cmdBuf->getVkCommandBuffer(1).pipelineBarrier(
				vk::PipelineStageFlagBits::eColorAttachmentOutput,
				vk::PipelineStageFlagBits::eBottomOfPipe,
				vk::DependencyFlagBits(),
				0, nullptr,
				0, nullptr,
				1, &barrier);
		}

		//end CommandBuffer
		m_cmdBuf->getVkCommandBuffer(1).end();

		m_queue->submit(m_cmdBuf->getVkCommandBuffer(1),m_semaphore);

		//wait Queue
		m_queue->wait();

		m_queue->present(m_swapchain->getVkSwapchainKHR(), m_bufferIndex);

		//wait Device
		m_device->getVkDevice(0).waitIdle();

		std::cout << "BufferIndex:" << m_bufferIndex << std::endl;
        std::cout << "Time:" << glfwGetTime() << std::endl;
		++m_count;
		return true;
	}
};


int main(
	int argc,
	char **argv)
{
	bool validate = true; // Use vulkan validation layer + debug report?
	if (argc > 1)
	{
		if (strcmp("-novalidate", argv[1]) == 0) {
			validate = false;
		}
	}
	simpleVulkan::Application* app = new TestApplication();
	if (app->create("test", 400, 400, validate))
	{
		app->run();
		app->destroy();
	}
}
