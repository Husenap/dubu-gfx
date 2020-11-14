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
	GraphicsPipeline(const CreateInfo& ci);

	[[nodiscard]] const vk::Pipeline& GetPipeline() const { return *mPipeline; }

private:
	vk::UniquePipeline mPipeline;
};

}  // namespace dubu::gfx