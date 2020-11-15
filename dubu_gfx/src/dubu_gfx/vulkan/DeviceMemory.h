#pragma once

namespace dubu::gfx {

class DeviceMemory {
public:
	struct CreateInfo {
		vk::Device              device           = {};
		vk::PhysicalDevice      physicalDevice   = {};
		vk::DeviceSize          allocationSize   = {};
		uint32_t                typeFilter       = {};
		vk::MemoryPropertyFlags memoryProperties = {};
	};

public:
	DeviceMemory(const CreateInfo& createInfo);

	[[nodiscard]] void* Map(std::size_t numBytes);
	void                Unmap();

	[[nodiscard]] const vk::DeviceMemory& GetDeviceMemory() const {
		return *mMemory;
	}

private:
	std::optional<uint32_t> FindMemoryType();

	CreateInfo             mCreateInfo;
	vk::UniqueDeviceMemory mMemory;
};

}  // namespace dubu::gfx