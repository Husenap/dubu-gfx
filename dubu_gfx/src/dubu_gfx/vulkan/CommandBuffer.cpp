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
    const std::vector<DrawingCommand>& drawingCommands) {
	for (std::size_t i = 0; i < mCommandBuffers.size(); ++i) {
		mCommandBuffers[i]->begin(vk::CommandBufferBeginInfo{});

		internal::DrawingCommandVisitor visitor(*mCommandBuffers[i], i);

		for (const auto& drawingCommand : drawingCommands) {
			std::visit(visitor, drawingCommand);
		}

		mCommandBuffers[i]->end();
	}
}

}  // namespace dubu::gfx