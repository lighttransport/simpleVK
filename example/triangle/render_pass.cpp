#include"render_pass.h"

namespace simpleVulkan
{
    RenderPass::RenderPass()
    {
    }
    RenderPass::~RenderPass()
    {
    }

    Result RenderPass::create(vk::Device device,vk::Format colorFormat,vk::Format depthFormat)
    {
        m_device = device;

        vk::Result result;
        //init AttachmentDescription
        vk::AttachmentDescription attachments[2];
        attachments[0].setFormat(colorFormat);
        attachments[0].setSamples(vk::SampleCountFlagBits::e1);
        attachments[0].setLoadOp(vk::AttachmentLoadOp::eClear);
        attachments[0].setStoreOp(vk::AttachmentStoreOp::eStore);
        attachments[0].setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
        attachments[0].setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        attachments[0].setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal);
        attachments[0].setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);
        attachments[0].setFlags(vk::AttachmentDescriptionFlagBits());
        attachments[1].setFormat(depthFormat);
        attachments[1].setSamples(vk::SampleCountFlagBits::e1);
        attachments[1].setLoadOp(vk::AttachmentLoadOp::eClear);
        attachments[1].setStoreOp(vk::AttachmentStoreOp::eDontCare);
        attachments[1].setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
        attachments[1].setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        attachments[1].setInitialLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
        attachments[1].setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
        attachments[1].setFlags(vk::AttachmentDescriptionFlagBits());

        //init AttachmentReference
        vk::AttachmentReference colorRef;
        colorRef.setAttachment(0);
        colorRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);
        vk::AttachmentReference depthRef;
        depthRef.setAttachment(1);
        depthRef.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
  
        //init SubpassDescription
        vk::SubpassDescription subpassDesc;
        subpassDesc.setFlags(vk::SubpassDescriptionFlagBits());
        subpassDesc.setInputAttachmentCount(0);
        subpassDesc.setPInputAttachments(nullptr);
        subpassDesc.setColorAttachmentCount(1);
        subpassDesc.setPColorAttachments(&colorRef);
        subpassDesc.setPResolveAttachments(nullptr);
        subpassDesc.setPDepthStencilAttachment(&depthRef);
        subpassDesc.setPDepthStencilAttachment(nullptr);
        subpassDesc.setPreserveAttachmentCount(0);
        subpassDesc.setPPreserveAttachments(nullptr);

        //init RenderPassCreateInfo
        vk::RenderPassCreateInfo renderPassInfo;
        renderPassInfo.setFlags(vk::RenderPassCreateFlagBits());
        renderPassInfo.setAttachmentCount(2);
        renderPassInfo.setPAttachments(attachments);
        renderPassInfo.setSubpassCount(1);
        renderPassInfo.setPSubpasses(&subpassDesc);
        renderPassInfo.setDependencyCount(0);
        renderPassInfo.setPDependencies(nullptr);

        //create RenderPass
        result = m_device.createRenderPass(&renderPassInfo,nullptr,&m_renderPass);
        return result;
    }

    void RenderPass::destroy()
    {
        m_device.destroyRenderPass(m_renderPass,nullptr);
    }

    vk::RenderPass& RenderPass::getVkRenderPass()
    {
        return m_renderPass;
    }
}
