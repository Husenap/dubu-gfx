#include "Texture.h"

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

void Texture::LoadFromFile(const CreateInfo&            createInfo,
                           const std::filesystem::path& filepath) {
	int      textureChannels;
	stbi_uc* pixels = stbi_load(filepath.string().c_str(),
	                            &mDimensions.x,
	                            &mDimensions.y,
	                            &textureChannels,
	                            STBI_rgb_alpha);

	if (!pixels) {
		DUBU_LOG_FATAL("Failed to load texure: {}", filepath);
	}

	auto imageSize =
	    static_cast<std::size_t>(mDimensions.x * mDimensions.y * 4);

	LoadFromMemory(createInfo, mDimensions, pixels, imageSize);

	stbi_image_free(pixels);
}

void Texture::LoadFromCompressedMemory(const CreateInfo& createInfo,
                                       const void*       data,
                                       std::size_t       numBytes) {
	int      textureChannels;
	stbi_uc* pixels =
	    stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(data),
	                          static_cast<int>(numBytes),
	                          &mDimensions.x,
	                          &mDimensions.y,
	                          &textureChannels,
	                          STBI_rgb_alpha);

	if (!pixels) {
		DUBU_LOG_FATAL("Failed to load texure from compressed memory!");
	}

	auto imageSize =
	    static_cast<std::size_t>(mDimensions.x * mDimensions.y * 4);

	LoadFromMemory(createInfo, mDimensions, pixels, imageSize);

	stbi_image_free(pixels);
}

void Texture::LoadFromMemory(const CreateInfo& createInfo,
                             glm::ivec2        dimensions,
                             const void*       data,
                             std::size_t       numBytes) {
	mDimensions = dimensions;

	uint32_t mipLevels =
	    static_cast<uint32_t>(
	        std::floor(std::log2(std::max(mDimensions.x, mDimensions.y)))) +
	    1;

	dubu::gfx::Buffer stagingBuffer(dubu::gfx::Buffer::CreateInfo{
	    .device           = createInfo.device,
	    .physicalDevice   = createInfo.physicalDevice,
	    .size             = static_cast<uint32_t>(numBytes),
	    .usage            = vk::BufferUsageFlagBits::eTransferSrc,
	    .sharingMode      = vk::SharingMode::eExclusive,
	    .memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
	                        vk::MemoryPropertyFlagBits::eHostCoherent,
	});

	stagingBuffer.SetData(data, numBytes);

	mImage = std::make_unique<dubu::gfx::Image>(dubu::gfx::Image::CreateInfo{
	    .device         = createInfo.device,
	    .physicalDevice = createInfo.physicalDevice,
	    .imageInfo =
	        {
	            .imageType = vk::ImageType::e2D,
	            .format    = vk::Format::eR8G8B8A8Srgb,
	            .extent =
	                {
	                    .width  = static_cast<uint32_t>(mDimensions.x),
	                    .height = static_cast<uint32_t>(mDimensions.y),
	                    .depth  = 1,
	                },
	            .mipLevels   = mipLevels,
	            .arrayLayers = 1,
	            .samples     = vk::SampleCountFlagBits::e1,
	            .tiling      = vk::ImageTiling::eOptimal,
	            .usage       = vk::ImageUsageFlagBits::eTransferSrc |
	                     vk::ImageUsageFlagBits::eTransferDst |
	                     vk::ImageUsageFlagBits::eSampled,
	            .sharingMode   = vk::SharingMode::eExclusive,
	            .initialLayout = vk::ImageLayout::eUndefined,
	        },
	    .memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
	    .aspectMask       = vk::ImageAspectFlagBits::eColor,
	});

	mImage->TransitionImageLayout(vk::ImageLayout::eUndefined,
	                              vk::ImageLayout::eTransferDstOptimal,
	                              createInfo.queueFamilies,
	                              createInfo.graphicsQueue);

	mImage->SetData(stagingBuffer.GetBuffer(),
	                createInfo.queueFamilies,
	                createInfo.graphicsQueue,
	                static_cast<uint32_t>(mDimensions.x),
	                static_cast<uint32_t>(mDimensions.y));

	mImage->GenerateMipChain(createInfo.queueFamilies,
	                         createInfo.graphicsQueue,
	                         mDimensions.x,
	                         mDimensions.y);

	mImage->TransitionImageLayout(vk::ImageLayout::eTransferSrcOptimal,
	                              vk::ImageLayout::eShaderReadOnlyOptimal,
	                              createInfo.queueFamilies,
	                              createInfo.graphicsQueue);

	mSampler =
	    std::make_unique<dubu::gfx::Sampler>(dubu::gfx::Sampler::CreateInfo{
	        .device      = createInfo.device,
	        .filter      = vk::Filter::eLinear,
	        .addressMode = vk::SamplerAddressMode::eRepeat,
	    });

	mImageInfo = {
	    .sampler     = mSampler->GetSampler(),
	    .imageView   = mImage->GetImageView(),
	    .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
	};
}
