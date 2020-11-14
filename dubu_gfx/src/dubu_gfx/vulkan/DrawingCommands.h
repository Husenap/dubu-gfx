#pragma once

namespace dubu::gfx::DrawingCommands {

struct Custom {
	std::function<void(const vk::CommandBuffer&, std::size_t)> customFunction =
	    {};
};

struct BeginRenderPass {
	vk::RenderPass               renderPass   = {};
	std::vector<vk::Framebuffer> framebuffers = {};
	vk::Rect2D                   renderArea   = {};
	vk::ClearValue               clearColor   = {};
};
struct EndRenderPass {};

struct BindPipeline {
	vk::Pipeline          pipeline  = {};
	vk::PipelineBindPoint bindPoint = {};
};

struct SetViewport {
	std::vector<vk::Viewport> viewports = {};
};

struct BindVertexBuffers {
	std::vector<vk::Buffer>     buffers = {};
	std::vector<vk::DeviceSize> offsets = {};
};

struct BindIndexBuffer {
	vk::Buffer     buffer    = {};
	vk::DeviceSize offset    = {};
	vk::IndexType  indexType = {};
};

struct BindDescriptorSets {
	vk::PipelineBindPoint          bindPoint      = {};
	vk::PipelineLayout             pipelineLayout = {};
	uint32_t                       firstSet       = {};
	std::vector<vk::DescriptorSet> descriptorSets = {};
	std::vector<uint32_t>          dynamicOffsets = {};
};

struct Draw {
	uint32_t vertexCount   = {};
	uint32_t instanceCount = {};
	uint32_t firstVertex   = {};
	uint32_t firstInstance = {};
};

struct DrawIndexed {
	uint32_t indexCount    = {};
	uint32_t instanceCount = {};
	uint32_t firstIndex    = {};
	uint32_t vertexOffset  = {};
	uint32_t firstInstance = {};
};

}  // namespace dubu::gfx::DrawingCommands
