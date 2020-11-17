#pragma once

#include <dubu_gfx/dubu_gfx.h>
#include <glm/glm.hpp>

class Texture {
public:
	struct CreateInfo {
		vk::Device                    device;
		vk::PhysicalDevice            physicalDevice;
		dubu::gfx::QueueFamilyIndices queueFamilies;
		vk::Queue                     graphicsQueue;
	};

public:
	void LoadFromFile(const CreateInfo&            createInfo,
	                  const std::filesystem::path& filepath);
	void LoadFromCompressedMemory(const CreateInfo& createInfo,
	                              const void*       data,
	                              std::size_t       numBytes);
	void LoadFromMemory(const CreateInfo& createInfo,
	                    glm::ivec2        dimensions,
	                    const void*       data,
	                    std::size_t       numBytes);

	[[nodiscard]] const vk::ImageView& GetImageView() const {
		return mImage->GetImageView();
	}

	[[nodiscard]] operator bool() const { return mImage.operator bool(); }

private:
	glm::ivec2 mDimensions;

	std::unique_ptr<dubu::gfx::Image> mImage;
};