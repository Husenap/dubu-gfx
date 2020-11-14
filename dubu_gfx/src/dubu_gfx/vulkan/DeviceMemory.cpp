#include "DeviceMemory.h"

namespace dubu::gfx {

DeviceMemory::DeviceMemory(const CreateInfo& createInfo)
    : mCreateInfo(createInfo) {
	mMemory = createInfo.device.allocateMemoryUnique(vk::MemoryAllocateInfo{
	    .allocationSize  = createInfo.allocationSize,
	    .memoryTypeIndex = *FindMemoryType(),
	});
}

void* DeviceMemory::Map(std::size_t numBytes) {
	void* data = nullptr;

	auto result = mCreateInfo.device.mapMemory(
	    *mMemory, 0, numBytes, vk::MemoryMapFlagBits(0), &data);

	if (result != vk::Result::eSuccess) {
		DUBU_LOG_ERROR("Failed to map memory!");
		return nullptr;
	}

	return data;
}

void DeviceMemory::Unmap() {
	mCreateInfo.device.unmapMemory(*mMemory);
}

std::optional<uint32_t> DeviceMemory::FindMemoryType() {
	auto memProperties = mCreateInfo.physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
		if (mCreateInfo.typeFilter & (1ui32 << i) &&
		    (memProperties.memoryTypes[i].propertyFlags &
		     mCreateInfo.memoryProperties) == mCreateInfo.memoryProperties) {
			return i;
		}
	}

	DUBU_LOG_FATAL("Failed to find suitable memory type!");

	return std::nullopt;
}

}  // namespace dubu::gfx