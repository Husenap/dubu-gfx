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

struct Draw {
	uint32_t vertexCount   = {};
	uint32_t instanceCount = {};
	uint32_t firstVertex   = {};
	uint32_t firstInstance = {};
};

}  // namespace dubu::gfx::DrawingCommands
