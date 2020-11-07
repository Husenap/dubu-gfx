#pragma once

#include "DrawingCommands.h"

namespace dubu::gfx {

using DrawingCommand = std::variant<DrawingCommands::BeginRenderPass,
                                    DrawingCommands::EndRenderPass,
                                    DrawingCommands::BindPipeline,
                                    DrawingCommands::SetViewport,
                                    DrawingCommands::Draw>;

namespace internal {

class DrawingCommandVisitor {
public:
	DrawingCommandVisitor(vk::CommandBuffer& commandBuffer,
	                      std::size_t        commandBufferIndex)
	    : mCommandBuffer(commandBuffer)
	    , mCommandBufferIndex(commandBufferIndex) {}

	void operator()(const DrawingCommand&) {
		throw std::runtime_error("DrawingCommand Type Missing Implementation!");
	}

	void operator()(const DrawingCommands::BeginRenderPass& cmd);
	void operator()(const DrawingCommands::EndRenderPass& cmd);
	void operator()(const DrawingCommands::BindPipeline& cmd);
	void operator()(const DrawingCommands::SetViewport& cmd);
	void operator()(const DrawingCommands::Draw& cmd);

private:
	vk::CommandBuffer& mCommandBuffer;
	std::size_t        mCommandBufferIndex;
};

}  // namespace internal

}  // namespace dubu::gfx