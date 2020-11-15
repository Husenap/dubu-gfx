#pragma once

#include "DeviceMemory.h"
#include "QueueFamilyIndices.h"

namespace dubu::gfx {

class Buffer {
public:
	struct CreateInfo {
		vk::Device              device           = {};
		vk::PhysicalDevice      physicalDevice   = {};
		uint32_t                size             = {};
		vk::BufferUsageFlags    usage            = {};
		vk::SharingMode         sharingMode      = {};
		vk::MemoryPropertyFlags memoryProperties = {};
	};

public:
	Buffer(const CreateInfo& createInfo);

	template <typename T>
	void SetData(const std::vector<T>& data) {
		SetData(reinterpret_cast<const void*>(data.data()),
		        data.size() * sizeof(T));
	}

	template <typename T>
	void SetData(const T& data) {
		SetData(reinterpret_cast<const void*>(&data), sizeof(T));
	}

	void SetData(const void* bytes, std::size_t numBytes);
	void SetData(const vk::Buffer&         buffer,
	             const QueueFamilyIndices& queueFamilies,
	             const vk::Queue&          graphicsQueue);

	[[nodiscard]] const vk::Buffer& GetBuffer() const { return *mBuffer; }

private:
	CreateInfo mCreateInfo;

	vk::UniqueBuffer              mBuffer;
	std::unique_ptr<DeviceMemory> mMemory;
};

}  // namespace dubu::gfx