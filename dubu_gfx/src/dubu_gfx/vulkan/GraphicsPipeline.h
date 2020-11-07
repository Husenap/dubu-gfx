#pragma once

namespace dubu::gfx {

class GraphicsPipeline {
public:
	struct CreateInfo {
		vk::Device                                     device             = {};
		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages       = {};
		vk::PipelineVertexInputStateCreateInfo         vertexInputState   = {};
		vk::PipelineInputAssemblyStateCreateInfo       inputAssemblyState = {};
		vk::PipelineViewportStateCreateInfo            viewportState      = {};
		vk::PipelineRasterizationStateCreateInfo       rasterizationState = {};
		vk::PipelineMultisampleStateCreateInfo         multisampleState   = {};
		vk::PipelineDepthStencilStateCreateInfo        depthStencilState  = {};
		vk::PipelineColorBlendStateCreateInfo          colorBlendState    = {};
		vk::PipelineDynamicStateCreateInfo             dynamicState       = {};
		vk::PipelineLayout                             pipelineLayout     = {};
		vk::RenderPass                                 renderPass         = {};
		uint32_t                                       subpass            = {};
		vk::Pipeline                                   basePipelineHandle = {};
		int32_t                                        basePipelineIndex  = {};
	};

public:
	GraphicsPipeline(const CreateInfo& ci) {
		mPipeline = ci.device
		                .createGraphicsPipelineUnique(
		                    nullptr,
		                    vk::GraphicsPipelineCreateInfo{
		                        .stageCount = static_cast<uint32_t>(
		                            ci.shaderStages.size()),
		                        .pStages             = ci.shaderStages.data(),
		                        .pVertexInputState   = &ci.vertexInputState,
		                        .pInputAssemblyState = &ci.inputAssemblyState,
		                        .pViewportState      = &ci.viewportState,
		                        .pRasterizationState = &ci.rasterizationState,
		                        .pMultisampleState   = &ci.multisampleState,
		                        .pDepthStencilState  = &ci.depthStencilState,
		                        .pColorBlendState    = &ci.colorBlendState,
		                        .pDynamicState       = &ci.dynamicState,
		                        .layout              = ci.pipelineLayout,
		                        .renderPass          = ci.renderPass,
		                        .subpass             = ci.subpass,
		                        .basePipelineHandle  = ci.basePipelineHandle,
		                        .basePipelineIndex   = ci.basePipelineIndex,
		                    })
		                .value;
	}

	vk::Pipeline GetPipeline() const { return *mPipeline; }

private:
	vk::UniquePipeline mPipeline;
};

}  // namespace dubu::gfx