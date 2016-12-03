#include"graphics_pipeline.h"

namespace simpleVulkan
{
    GraphicsPipeline::GraphicsPipeline()
    {
    }

    GraphicsPipeline::~GraphicsPipeline()
    {
    }

    Result GraphicsPipeline::create(
            vk::Device device,
            vk::ShaderModule vertexShader,
            vk::ShaderModule fragmentShader,
            vk::DescriptorSetLayout setLayout,
            const std::vector<vk::VertexInputBindingDescription>& vertexBindings,
            const std::vector<vk::VertexInputAttributeDescription>& vertexAttributes,
            vk::Viewport viewport,
            vk::Rect2D scissor,
            vk::RenderPass renderPass)
    {
        vk::Result result;

        m_device = device;


        //init PipelineCacheCreateInfo
        vk::PipelineCacheCreateInfo cacheInfo;
        cacheInfo.setFlags(vk::PipelineCacheCreateFlagBits());
        cacheInfo.setInitialDataSize(0);
        cacheInfo.setPInitialData(nullptr);

        //create PipelineCache
        result = m_device.createPipelineCache(
                &cacheInfo,
                nullptr,
                &m_pipelineCache);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }

        //init PipelineShaderStageCreateInfo
        vk::PipelineShaderStageCreateInfo stageInfos[2];
        stageInfos[0].setFlags(vk::PipelineShaderStageCreateFlagBits());
        stageInfos[0].setStage(vk::ShaderStageFlagBits::eVertex);
        stageInfos[0].setModule(vertexShader);
        stageInfos[0].setPName("main");
        stageInfos[0].setPSpecializationInfo(nullptr);
        stageInfos[1].setFlags(vk::PipelineShaderStageCreateFlagBits());
        stageInfos[1].setStage(vk::ShaderStageFlagBits::eFragment);
        stageInfos[1].setModule(fragmentShader);
        stageInfos[1].setPName("main");
        stageInfos[1].setPSpecializationInfo(nullptr);

        //init PipelineLayoutCreateInfo
        vk::PipelineLayoutCreateInfo layoutInfo;
        layoutInfo.setPushConstantRangeCount(0);
        layoutInfo.setPPushConstantRanges(nullptr);
        layoutInfo.setSetLayoutCount(1);
        layoutInfo.setPSetLayouts(&setLayout);

        result = m_device.createPipelineLayout(
                &layoutInfo,
                nullptr,
                &m_pipelineLayout);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }

        //init VertexInputStateCreateInfo
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
        vertexInputInfo.setFlags(vk::PipelineVertexInputStateCreateFlagBits());
        vertexInputInfo.setVertexBindingDescriptionCount(vertexBindings.size());
        vertexInputInfo.setPVertexBindingDescriptions(vertexBindings.data());
        vertexInputInfo.setVertexAttributeDescriptionCount(vertexAttributes.size());
        vertexInputInfo.setPVertexAttributeDescriptions(vertexAttributes.data());

        //init PipelineInputAssemblyStateCreateInfo
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        inputAssemblyInfo.setFlags(vk::PipelineInputAssemblyStateCreateFlagBits());
        inputAssemblyInfo.setTopology(vk::PrimitiveTopology::eTriangleList);
        inputAssemblyInfo.setPrimitiveRestartEnable(false);

        //init PipelineViewportStateCreateInfo
        vk::PipelineViewportStateCreateInfo viewportInfo;
        viewportInfo.setFlags(vk::PipelineViewportStateCreateFlagBits());
        viewportInfo.setViewportCount(1);
        viewportInfo.setPViewports(&viewport);
        viewportInfo.setScissorCount(1);
        viewportInfo.setPScissors(&scissor);

        //init PipelineRasterizationStateCreateInfo
        vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
        rasterizationInfo.setFlags(vk::PipelineRasterizationStateCreateFlagBits());
        rasterizationInfo.setDepthClampEnable(false);
        rasterizationInfo.setRasterizerDiscardEnable(false);
        rasterizationInfo.setPolygonMode(vk::PolygonMode::eFill);
        rasterizationInfo.setCullMode(vk::CullModeFlagBits::eNone);
        rasterizationInfo.setFrontFace(vk::FrontFace::eCounterClockwise);
        rasterizationInfo.setDepthBiasEnable(false);
        rasterizationInfo.setDepthBiasConstantFactor(0.0f);
        rasterizationInfo.setDepthBiasClamp(0.0f);
        rasterizationInfo.setDepthBiasSlopeFactor(0.0f);
        rasterizationInfo.setLineWidth(1.0f);

        //init PipelineMultisampleStateCreateInfo
        vk::PipelineMultisampleStateCreateInfo multisampleInfo;
        multisampleInfo.setFlags(vk::PipelineMultisampleStateCreateFlagBits());
        multisampleInfo.setRasterizationSamples(vk::SampleCountFlagBits::e1);
        multisampleInfo.setSampleShadingEnable(false);
        multisampleInfo.setMinSampleShading(0.0f);
        multisampleInfo.setPSampleMask(nullptr);
        multisampleInfo.setAlphaToCoverageEnable(false);
        multisampleInfo.setAlphaToOneEnable(false);

        //init StencilOpState
        vk::StencilOpState stencilState;
        stencilState.setFailOp(vk::StencilOp::eKeep);
        stencilState.setPassOp(vk::StencilOp::eKeep);
        stencilState.setDepthFailOp(vk::StencilOp::eKeep);
        stencilState.setCompareOp(vk::CompareOp::eNever);
        stencilState.setCompareMask(0);
        stencilState.setWriteMask(0);
        stencilState.setReference(0);

        //init PipelineDepthStencilStateCreateInfo
        vk::PipelineDepthStencilStateCreateInfo depthInfo;
        depthInfo.setFlags(vk::PipelineDepthStencilStateCreateFlagBits());
        depthInfo.setDepthTestEnable(true); //debug
        depthInfo.setDepthWriteEnable(true);
        depthInfo.setDepthCompareOp(vk::CompareOp::eLessOrEqual);
        depthInfo.setDepthBoundsTestEnable(false);
        depthInfo.setStencilTestEnable(false);
        depthInfo.setFront(stencilState);
        depthInfo.setBack(stencilState);
        depthInfo.setMinDepthBounds(0.0f);
        depthInfo.setMaxDepthBounds(0.0f);

        //init PipelineColorBlendAttachmentState
        vk::PipelineColorBlendAttachmentState blendState;
        blendState.setBlendEnable(false);
        blendState.setSrcColorBlendFactor(vk::BlendFactor::eZero);
        blendState.setDstColorBlendFactor(vk::BlendFactor::eZero);
        blendState.setColorBlendOp(vk::BlendOp::eAdd);
        blendState.setSrcAlphaBlendFactor(vk::BlendFactor::eZero);
        blendState.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
        blendState.setAlphaBlendOp(vk::BlendOp::eAdd);
        blendState.setColorWriteMask(
                vk::ColorComponentFlagBits::eR |
                vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB |
                vk::ColorComponentFlagBits::eA );

        //init PipelineColorBlendStateCreateInfo
        vk::PipelineColorBlendStateCreateInfo blendInfo;
        blendInfo.setFlags(vk::PipelineColorBlendStateCreateFlagBits());
        blendInfo.setLogicOpEnable(false);
        blendInfo.setLogicOp(vk::LogicOp::eClear);
        blendInfo.setAttachmentCount(1);
        blendInfo.setPAttachments(&blendState);
        blendInfo.setBlendConstants(std::array<float,4>{0.0f,0.0f,0.0f,0.0f});

        //init GraphicPipelineCreateInfo
        vk::GraphicsPipelineCreateInfo pipelineInfo;
        pipelineInfo.setFlags(vk::PipelineCreateFlagBits());
        pipelineInfo.setStageCount(2);
        pipelineInfo.setPStages(stageInfos);
        pipelineInfo.setPVertexInputState(&vertexInputInfo);
        pipelineInfo.setPInputAssemblyState(&inputAssemblyInfo);
        pipelineInfo.setPTessellationState(nullptr);
        pipelineInfo.setPViewportState(&viewportInfo);
        pipelineInfo.setPRasterizationState(&rasterizationInfo);
        pipelineInfo.setPMultisampleState(&multisampleInfo);
        pipelineInfo.setPDepthStencilState(&depthInfo);
        pipelineInfo.setPColorBlendState(&blendInfo);
        pipelineInfo.setPDynamicState(nullptr);
        pipelineInfo.setLayout(m_pipelineLayout);
        pipelineInfo.setRenderPass(renderPass);
        pipelineInfo.setSubpass(0);
        pipelineInfo.setBasePipelineHandle(vk::Pipeline());
        pipelineInfo.setBasePipelineIndex(0);

        //create GraphicPipeline
        result = m_device.createGraphicsPipelines(
                m_pipelineCache,
                1,
                &pipelineInfo,
                nullptr,
                &m_pipeline);
        return result;
    }

void GraphicsPipeline::destroy()
{
    m_device.destroyPipeline(m_pipeline,nullptr);
    m_device.destroyPipelineLayout(m_pipelineLayout,nullptr);
    m_device.destroyPipelineCache(m_pipelineCache,nullptr);
}

vk::PipelineLayout& GraphicsPipeline::getVkPipelineLayout()
{
    return m_pipelineLayout;
}

    vk::Pipeline& GraphicsPipeline::getVkPipeline()
    {
        return m_pipeline;
    }
}
