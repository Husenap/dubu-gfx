#include "DrawingCommand.h"

namespace dubu::gfx::internal {

void DrawingCommandVisitor::operator()(const DrawingCommands::Custom& cmd) {
	cmd.customFunction(mCommandBuffer, mCommandBufferIndex);
}

void DrawingCommandVisitor::operator()(
    const DrawingCommands::BeginRenderPass& cmd) {
	mCommandBuffer.beginRenderPass(
	    vk::RenderPassBeginInfo{
	        .renderPass      = cmd.renderPass,
	        .framebuffer     = cmd.framebuffers[mCommandBufferIndex],
	        .renderArea      = cmd.renderArea,
	        .clearValueCount = 1,
	        .pClearValues    = &cmd.clearColor,
	    },
	    vk::SubpassContents::eInline);
}

void DrawingCommandVisitor::operator()(const DrawingCommands::EndRenderPass&) {
	mCommandBuffer.endRenderPass();
}

void DrawingCommandVisitor::operator()(
    const DrawingCommands::BindPipeline& cmd) {
	mCommandBuffer.bindPipeline(cmd.bindPoint, cmd.pipeline);
}

void DrawingCommandVisitor::operator()(
    const DrawingCommands::SetViewport& cmd) {
	mCommandBuffer.setViewport(0, cmd.viewports);
}

void DrawingCommandVisitor::operator()(
    const DrawingCommands::BindVertexBuffers& cmd) {
	mCommandBuffer.bindVertexBuffers(0, cmd.buffers, cmd.offsets);
}

void DrawingCommandVisitor::operator()(const DrawingCommands::Draw& cmd) {
	mCommandBuffer.draw(
	    cmd.vertexCount, cmd.instanceCount, cmd.firstVertex, cmd.firstInstance);
}

}  // namespace dubu::gfx::internal