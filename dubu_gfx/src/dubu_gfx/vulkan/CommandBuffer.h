#pragma once

#include "DrawingCommand.h"

namespace dubu::gfx {

class CommandBuffer {
public:
	struct CreateInfo {
		vk::Device      device      = {};
		vk::CommandPool commandPool = {};
		uint32_t        bufferCount = {};
	};

public:
	CommandBuffer(const CreateInfo& createInfo);

	void RecordCommands(std::size_t                        bufferIndex,
	                    const std::vector<DrawingCommand>& drawingCommands);
	void RecordCommandsForAllBuffers(
	    const std::vector<DrawingCommand>& drawingCommands);

	const vk::CommandBuffer& GetCommandBuffer(std::size_t index) const {
		return *mCommandBuffers[index];
	}

private:
	std::vector<vk::UniqueCommandBuffer> mCommandBuffers;
};

}  // namespace dubu::gfx