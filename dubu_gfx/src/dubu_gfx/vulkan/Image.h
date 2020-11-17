#pragma once

#include "DeviceMemory.h"
#include "QueueFamilyIndices.h"

namespace dubu::gfx {

class Image {
public:
	struct CreateInfo {
		vk::Device              device           = {};
		vk::PhysicalDevice      physicalDevice   = {};
		vk::ImageCreateInfo     imageInfo        = {};
		vk::MemoryPropertyFlags memoryProperties = {};
		vk::ImageAspectFlags    aspectMask       = {};
	};

public:
	Image(const CreateInfo& createInfo);

	void SetData(const vk::Buffer&         buffer,
	             const QueueFamilyIndices& queueFamilies,
	             const vk::Queue&          graphicsQueue,
	             uint32_t                  width,
	             uint32_t                  height);

	void TransitionImageLayout(vk::ImageLayout           oldLayout,
	                           vk::ImageLayout           newLayout,
	                           const QueueFamilyIndices& queueFamilies,
	                           const vk::Queue&          graphicsQueue);

	void GenerateMipChain(const QueueFamilyIndices& queueFamilies,
	                      const vk::Queue&          graphicsQueue,
	                      uint32_t                  width,
	                      uint32_t                  height);

	[[nodiscard]] const vk::Image&     GetImage() const { return *mImage; }
	[[nodiscard]] const vk::ImageView& GetImageView() const {
		return *mImageView;
	}
	[[nodiscard]] vk::Format GetFormat() const {
		return mCreateInfo.imageInfo.format;
	}

private:
	CreateInfo mCreateInfo;

	vk::UniqueImage               mImage;
	vk::UniqueImageView           mImageView;
	std::unique_ptr<DeviceMemory> mMemory;
};

}  // namespace dubu::gfx