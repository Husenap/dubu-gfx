#include "Buffer.h"

namespace dubu::gfx {

Buffer::Buffer(const CreateInfo& createInfo)
    : mCreateInfo(createInfo) {
	mBuffer = createInfo.device.createBufferUnique(vk::BufferCreateInfo{
	    .size        = createInfo.size,
	    .usage       = createInfo.usage,
	    .sharingMode = createInfo.sharingMode,
	});

	auto memRequirements =
	    createInfo.device.getBufferMemoryRequirements(*mBuffer);

	mMemory = createInfo.device.allocateMemoryUnique(vk::MemoryAllocateInfo{
	    .allocationSize  = memRequirements.size,
	    .memoryTypeIndex = *FindMemoryType(memRequirements.memoryTypeBits,
	                                       createInfo.memoryProperties),
	});

	createInfo.device.bindBufferMemory(*mBuffer, *mMemory, 0);
}

void Buffer::SetData(const void* bytes, std::size_t numBytes) {
	void* data;

	auto result = mCreateInfo.device.mapMemory(
	    *mMemory, 0, numBytes, vk::MemoryMapFlagBits(0), &data);

	if (result != vk::Result::eSuccess) {
		DUBU_LOG_ERROR("Failed to map memory!");
		return;
	}

	std::memcpy(data, bytes, numBytes);

	mCreateInfo.device.unmapMemory(*mMemory);
}

std::optional<uint32_t> Buffer::FindMemoryType(
    uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
	auto memProperties = mCreateInfo.physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
		if (typeFilter & (1 << i) &&
		    (memProperties.memoryTypes[i].propertyFlags & properties) ==
		        properties) {
			return i;
		}
	}

	DUBU_LOG_FATAL("Failed to find suitable memory type!");

	return std::nullopt;
}

}  // namespace dubu::gfx