#include"queue.h"

#include"device.h"

namespace simpleVulkan
{
    Queue::Queue(){}
    Queue::~Queue(){}

    void Queue::init(vk::Device device)
    {
        m_device = device;
        m_device.getQueue(0,0,&m_queue);
    }

    Result Queue::submit(vk::CommandBuffer cmdBuffer)
    {
		vk::PipelineStageFlags stage = vk::PipelineStageFlagBits::eBottomOfPipe;
        vk::Result result;
        //init SubmitInfo
        vk::SubmitInfo submitInfo;
        submitInfo.setWaitSemaphoreCount(0);
        submitInfo.setPWaitSemaphores(nullptr);
        submitInfo.setSignalSemaphoreCount(0);
        submitInfo.setPSignalSemaphores(nullptr);
        submitInfo.setPWaitDstStageMask(&stage);
        submitInfo.setCommandBufferCount(1);
        submitInfo.setPCommandBuffers(&cmdBuffer);

        //submit queue
        result = m_queue.submit(1,&submitInfo,vk::Fence());
        return result;
    }

    Result Queue::submit(vk::CommandBuffer cmdBuffer,vk::Semaphore semaphore)
    {
		vk::PipelineStageFlags stage = vk::PipelineStageFlagBits::eBottomOfPipe;
        vk::Result result;
        //init SubmitInfo
        vk::SubmitInfo submitInfo;
        submitInfo.setWaitSemaphoreCount(1);
        submitInfo.setPWaitSemaphores(&semaphore);
        submitInfo.setSignalSemaphoreCount(0);
        submitInfo.setPSignalSemaphores(nullptr);
        submitInfo.setPWaitDstStageMask(&stage);
        submitInfo.setCommandBufferCount(1);
        submitInfo.setPCommandBuffers(&cmdBuffer);

        //submit queue
        result = m_queue.submit(1,&submitInfo,vk::Fence());
        return result;
    }
    Result Queue::wait()
    {
		m_queue.waitIdle();
		return Result::eSuccess;
    }
    Result Queue::present(vk::SwapchainKHR swapchain,uint32_t index)
    {
        vk::Result result;
        //init PresentInfoKHR
        vk::PresentInfoKHR presentInfo;
        presentInfo.setSwapchainCount(1);
        presentInfo.setPSwapchains(&swapchain);
        presentInfo.setPImageIndices(&index);
        presentInfo.setWaitSemaphoreCount(0);
        presentInfo.setPWaitSemaphores(nullptr);
        presentInfo.setPResults(nullptr);

        //present
        result = m_queue.presentKHR(&presentInfo);
        return result;
    }

    vk::Queue& Queue::getVkQueue()
    {
        return m_queue;
    }
}
