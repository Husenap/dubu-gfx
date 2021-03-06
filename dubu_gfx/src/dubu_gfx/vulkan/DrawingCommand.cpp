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
	        .clearValueCount = static_cast<uint32_t>(cmd.clearValues.size()),
	        .pClearValues    = cmd.clearValues.data(),
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

void DrawingCommandVisitor::operator()(
    const DrawingCommands::BindIndexBuffer& cmd) {
	mCommandBuffer.bindIndexBuffer(cmd.buffer, cmd.offset, cmd.indexType);
}

void DrawingCommandVisitor::operator()(
    const DrawingCommands::BindDescriptorSets& cmd) {
	mCommandBuffer.bindDescriptorSets(cmd.bindPoint,
	                                  cmd.pipelineLayout,
	                                  cmd.firstSet,
	                                  cmd.descriptorSets,
	                                  cmd.dynamicOffsets);
}

void DrawingCommandVisitor::operator()(const DrawingCommands::Draw& cmd) {
	mCommandBuffer.draw(
	    cmd.vertexCount, cmd.instanceCount, cmd.firstVertex, cmd.firstInstance);
}

void DrawingCommandVisitor::operator()(
    const DrawingCommands::DrawIndexed& cmd) {
	mCommandBuffer.drawIndexed(cmd.indexCount,
	                           cmd.instanceCount,
	                           cmd.firstIndex,
	                           cmd.vertexOffset,
	                           cmd.firstInstance);
}

}  // namespace dubu::gfx::internal