#include "CommandBuffer.h"

namespace dubu::gfx {

CommandBuffer::CommandBuffer(const CreateInfo& createInfo) {
	mCommandBuffers = createInfo.device.allocateCommandBuffersUnique(
	    vk::CommandBufferAllocateInfo{
	        .commandPool        = createInfo.commandPool,
	        .level              = vk::CommandBufferLevel::ePrimary,
	        .commandBufferCount = createInfo.bufferCount,
	    });
}

void CommandBuffer::RecordCommands(
    std::size_t                        bufferIndex,
    const std::vector<DrawingCommand>& drawingCommands) {
	mCommandBuffers[bufferIndex]->begin(vk::CommandBufferBeginInfo{});

	internal::DrawingCommandVisitor visitor(*mCommandBuffers[bufferIndex],
	                                        bufferIndex);

	for (const auto& drawingCommand : drawingCommands) {
		std::visit(visitor, drawingCommand);
	}

	mCommandBuffers[bufferIndex]->end();
}

void CommandBuffer::RecordCommandsForAllBuffers(
    const std::vector<DrawingCommand>& drawingCommands) {
	for (std::size_t i = 0; i < mCommandBuffers.size(); ++i) {
		RecordCommands(i, drawingCommands);
	}
}

}  // namespace dubu::gfx