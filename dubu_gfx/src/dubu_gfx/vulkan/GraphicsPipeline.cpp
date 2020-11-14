#include "GraphicsPipeline.h"

namespace dubu::gfx {

GraphicsPipeline::GraphicsPipeline(const CreateInfo& ci) {
	mPipeline =
	    ci.device
	        .createGraphicsPipelineUnique(
	            nullptr,
	            vk::GraphicsPipelineCreateInfo{
	                .stageCount = static_cast<uint32_t>(ci.shaderStages.size()),
	                .pStages    = ci.shaderStages.data(),
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

}  // namespace dubu::gfx