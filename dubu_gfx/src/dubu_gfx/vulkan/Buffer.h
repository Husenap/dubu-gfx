#pragma once

namespace dubu::gfx {

class Buffer {
public:
	struct CreateInfo {
		vk::Device              device;
		vk::PhysicalDevice      physicalDevice;
		uint32_t                size;
		vk::BufferUsageFlagBits usage;
		vk::SharingMode         sharingMode;
		vk::MemoryPropertyFlags memoryProperties;
	};

public:
	Buffer(const CreateInfo& createInfo);

	template <typename T>
	void SetData(const std::vector<T>& data) {
		SetData(reinterpret_cast<const void*>(data.data()),
		        data.size() * sizeof(T));
	}

	void SetData(const void* bytes, std::size_t numBytes);

	const vk::Buffer& GetBuffer() const { return *mBuffer; }

private:
	std::optional<uint32_t> FindMemoryType(uint32_t                typeFilter,
	                                       vk::MemoryPropertyFlags properties);

	CreateInfo mCreateInfo;

	vk::UniqueBuffer       mBuffer;
	vk::UniqueDeviceMemory mMemory;
};

}  // namespace dubu::gfx