#include "Image.h"

#include "CommandBuffer.h"
#include "CommandPool.h"

namespace dubu::gfx {

Image::Image(const CreateInfo& createInfo)
    : mCreateInfo(createInfo) {
	mImage = createInfo.device.createImageUnique(createInfo.imageInfo);

	auto memRequirements =
	    createInfo.device.getImageMemoryRequirements(*mImage);

	mMemory = std::make_unique<DeviceMemory>(DeviceMemory::CreateInfo{
	    .device           = createInfo.device,
	    .physicalDevice   = createInfo.physicalDevice,
	    .allocationSize   = memRequirements.size,
	    .typeFilter       = memRequirements.memoryTypeBits,
	    .memoryProperties = createInfo.memoryProperties,
	});

	createInfo.device.bindImageMemory(*mImage, mMemory->GetDeviceMemory(), 0);

	mImageView =
	    createInfo.device.createImageViewUnique(vk::ImageViewCreateInfo{
	        .image    = *mImage,
	        .viewType = vk::ImageViewType::e2D,
	        .format   = createInfo.imageInfo.format,
	        .subresourceRange =
	            {
	                .aspectMask     = createInfo.aspectMask,
	                .baseMipLevel   = 0,
	                .levelCount     = createInfo.imageInfo.mipLevels,
	                .baseArrayLayer = 0,
	                .layerCount     = 1,
	            },
	    });
}

void Image::SetData(const vk::Buffer&         buffer,
                    const QueueFamilyIndices& queueFamilies,
                    const vk::Queue&          graphicsQueue,
                    uint32_t                  width,
                    uint32_t                  height) {
	dubu::gfx::CommandPool commandPool(dubu::gfx::CommandPool::CreateInfo{
	    .device        = mCreateInfo.device,
	    .queueFamilies = queueFamilies,
	});

	dubu::gfx::CommandBuffer commandBuffer(dubu::gfx::CommandBuffer::CreateInfo{
	    .device      = mCreateInfo.device,
	    .commandPool = commandPool.GetCommandPool(),
	    .bufferCount = 1,
	});

	commandBuffer.GetCommandBuffer().begin({
	    .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
	});

	commandBuffer.GetCommandBuffer().copyBufferToImage(
	    buffer,
	    *mImage,
	    vk::ImageLayout::eTransferDstOptimal,
	    {
	        vk::BufferImageCopy{.bufferOffset      = 0,
	                            .bufferRowLength   = 0,
	                            .bufferImageHeight = 0,
	                            .imageSubresource =
	                                {
	                                    .aspectMask = mCreateInfo.aspectMask,
	                                    .mipLevel   = 0,
	                                    .baseArrayLayer = 0,
	                                    .layerCount     = 1,
	                                },
	                            .imageOffset = {0, 0, 0},
	                            .imageExtent = {width, height, 1}},
	    });

	commandBuffer.GetCommandBuffer().end();

	graphicsQueue.submit({{
	    .commandBufferCount = 1,
	    .pCommandBuffers    = &commandBuffer.GetCommandBuffer(),
	}});
	graphicsQueue.waitIdle();
}

void Image::TransitionImageLayout(vk::ImageLayout           oldLayout,
                                  vk::ImageLayout           newLayout,
                                  const QueueFamilyIndices& queueFamilies,
                                  const vk::Queue&          graphicsQueue) {
	dubu::gfx::CommandPool commandPool(dubu::gfx::CommandPool::CreateInfo{
	    .device        = mCreateInfo.device,
	    .queueFamilies = queueFamilies,
	});

	dubu::gfx::CommandBuffer commandBuffer(dubu::gfx::CommandBuffer::CreateInfo{
	    .device      = mCreateInfo.device,
	    .commandPool = commandPool.GetCommandPool(),
	    .bufferCount = 1,
	});

	commandBuffer.GetCommandBuffer().begin({
	    .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
	});

	vk::AccessFlags        sourceAccess;
	vk::AccessFlags        destinationAccess;
	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	switch (oldLayout) {
	case vk::ImageLayout::eUndefined:
		sourceAccess = {};
		sourceStage  = vk::PipelineStageFlagBits::eTopOfPipe;
		break;
	case vk::ImageLayout::eTransferSrcOptimal:
		sourceAccess = vk::AccessFlagBits::eTransferRead;
		sourceStage  = vk::PipelineStageFlagBits::eTransfer;
		break;
	case vk::ImageLayout::eTransferDstOptimal:
		sourceAccess = vk::AccessFlagBits::eTransferWrite;
		sourceStage  = vk::PipelineStageFlagBits::eTransfer;
		break;
	default:
		break;
	}

	switch (newLayout) {
	case vk::ImageLayout::eDepthStencilAttachmentOptimal:
		destinationAccess = vk::AccessFlagBits::eDepthStencilAttachmentRead |
		                    vk::AccessFlagBits::eDepthStencilAttachmentWrite;
		destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
		break;
	case vk::ImageLayout::eShaderReadOnlyOptimal:
		destinationAccess = vk::AccessFlagBits::eShaderRead;
		destinationStage  = vk::PipelineStageFlagBits::eFragmentShader;
		break;
	case vk::ImageLayout::eTransferSrcOptimal:
		destinationAccess = vk::AccessFlagBits::eTransferRead;
		destinationStage  = vk::PipelineStageFlagBits::eTransfer;
		break;
	case vk::ImageLayout::eTransferDstOptimal:
		destinationAccess = vk::AccessFlagBits::eTransferWrite;
		destinationStage  = vk::PipelineStageFlagBits::eTransfer;
		break;
	default:
		break;
	}

	commandBuffer.GetCommandBuffer().pipelineBarrier(
	    sourceStage,
	    destinationStage,
	    vk::DependencyFlags(0),
	    {},
	    {},
	    {vk::ImageMemoryBarrier{
	        .srcAccessMask       = sourceAccess,
	        .dstAccessMask       = destinationAccess,
	        .oldLayout           = oldLayout,
	        .newLayout           = newLayout,
	        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	        .image               = *mImage,
	        .subresourceRange =
	            {
	                .aspectMask     = mCreateInfo.aspectMask,
	                .baseMipLevel   = 0,
	                .levelCount     = mCreateInfo.imageInfo.mipLevels,
	                .baseArrayLayer = 0,
	                .layerCount     = 1,
	            },
	    }});

	commandBuffer.GetCommandBuffer().end();

	graphicsQueue.submit({{
	    .commandBufferCount = 1,
	    .pCommandBuffers    = &commandBuffer.GetCommandBuffer(),
	}});
	graphicsQueue.waitIdle();
}

void Image::GenerateMipChain(const QueueFamilyIndices& queueFamilies,
                             const vk::Queue&          graphicsQueue,
                             uint32_t                  width,
                             uint32_t                  height) {
	dubu::gfx::CommandPool commandPool(dubu::gfx::CommandPool::CreateInfo{
	    .device        = mCreateInfo.device,
	    .queueFamilies = queueFamilies,
	});

	dubu::gfx::CommandBuffer commandBuffer(dubu::gfx::CommandBuffer::CreateInfo{
	    .device      = mCreateInfo.device,
	    .commandPool = commandPool.GetCommandPool(),
	    .bufferCount = 1,
	});

	commandBuffer.GetCommandBuffer().begin({
	    .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
	});

	vk::ImageMemoryBarrier barrier{
	    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	    .image               = *mImage,
	    .subresourceRange =
	        {
	            .aspectMask     = mCreateInfo.aspectMask,
	            .levelCount     = 1,
	            .baseArrayLayer = 0,
	            .layerCount     = 1,
	        },
	};

	int32_t mipWidth  = static_cast<int32_t>(width);
	int32_t mipHeight = static_cast<int32_t>(height);

	for (uint32_t i = 1; i <= mCreateInfo.imageInfo.mipLevels; ++i) {
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout     = vk::ImageLayout::eTransferDstOptimal;
		barrier.newLayout     = vk::ImageLayout::eTransferSrcOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

		commandBuffer.GetCommandBuffer().pipelineBarrier(
		    vk::PipelineStageFlagBits::eTransfer,
		    vk::PipelineStageFlagBits::eTransfer,
		    vk::DependencyFlags(0),
		    {},
		    {},
		    {barrier});

		if (i == mCreateInfo.imageInfo.mipLevels) {
			break;
		}

		vk::ImageBlit blit{
		    .srcSubresource =
		        {
		            .aspectMask     = mCreateInfo.aspectMask,
		            .mipLevel       = i - 1,
		            .baseArrayLayer = 0,
		            .layerCount     = 1,
		        },
		    .srcOffsets =
		        std::array<vk::Offset3D, 2>{
		            vk::Offset3D{0, 0, 0},
		            vk::Offset3D{mipWidth, mipHeight, 1},
		        },
		    .dstSubresource =
		        {
		            .aspectMask     = mCreateInfo.aspectMask,
		            .mipLevel       = i,
		            .baseArrayLayer = 0,
		            .layerCount     = 1,
		        },
		    .dstOffsets =
		        std::array<vk::Offset3D, 2>{
		            vk::Offset3D{0, 0, 0},
		            vk::Offset3D{mipWidth > 1 ? mipWidth / 2 : 1,
		                         mipHeight > 1 ? mipHeight / 2 : 1,
		                         1},
		        },
		};

		commandBuffer.GetCommandBuffer().blitImage(
		    *mImage,
		    vk::ImageLayout::eTransferSrcOptimal,
		    *mImage,
		    vk::ImageLayout::eTransferDstOptimal,
		    {blit},
		    vk::Filter::eLinear);

		if (mipWidth > 1) {
			mipWidth /= 2;
		}
		if (mipHeight > 1) {
			mipHeight /= 2;
		}
	}

	commandBuffer.GetCommandBuffer().end();

	graphicsQueue.submit({{
	    .commandBufferCount = 1,
	    .pCommandBuffers    = &commandBuffer.GetCommandBuffer(),
	}});
	graphicsQueue.waitIdle();
}

}  // namespace dubu::gfx
