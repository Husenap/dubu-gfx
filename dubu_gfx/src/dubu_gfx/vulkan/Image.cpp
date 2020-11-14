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
	                .aspectMask     = vk::ImageAspectFlagBits::eColor,
	                .baseMipLevel   = 0,
	                .levelCount     = 1,
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

	commandBuffer.GetCommandBuffer(0).begin({
	    .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
	});

	commandBuffer.GetCommandBuffer(0).copyBufferToImage(
	    buffer,
	    *mImage,
	    vk::ImageLayout::eTransferDstOptimal,
	    {
	        vk::BufferImageCopy{
	            .bufferOffset      = 0,
	            .bufferRowLength   = 0,
	            .bufferImageHeight = 0,
	            .imageSubresource =
	                {
	                    .aspectMask     = vk::ImageAspectFlagBits::eColor,
	                    .mipLevel       = 0,
	                    .baseArrayLayer = 0,
	                    .layerCount     = 1,
	                },
	            .imageOffset = {0, 0, 0},
	            .imageExtent = {width, height, 1}},
	    });

	commandBuffer.GetCommandBuffer(0).end();

	graphicsQueue.submit({{
	    .commandBufferCount = 1,
	    .pCommandBuffers    = &commandBuffer.GetCommandBuffer(0),
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

	commandBuffer.GetCommandBuffer(0).begin({
	    .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
	});

	vk::AccessFlags        sourceAccess;
	vk::AccessFlags        destinationAccess;
	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oldLayout == vk::ImageLayout::eUndefined &&
	    newLayout == vk::ImageLayout::eTransferDstOptimal) {
		sourceAccess      = {};
		destinationAccess = vk::AccessFlagBits::eTransferWrite;
		sourceStage       = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage  = vk::PipelineStageFlagBits::eTransfer;
	} else if (oldLayout == vk::ImageLayout::eTransferDstOptimal &&
	           newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		sourceAccess      = vk::AccessFlagBits::eTransferWrite;
		destinationAccess = vk::AccessFlagBits::eShaderRead;
		sourceStage       = vk::PipelineStageFlagBits::eTransfer;
		destinationStage  = vk::PipelineStageFlagBits::eFragmentShader;
	}
	commandBuffer.GetCommandBuffer(0).pipelineBarrier(
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
	                .aspectMask     = vk::ImageAspectFlagBits::eColor,
	                .baseMipLevel   = 0,
	                .levelCount     = 1,
	                .baseArrayLayer = 0,
	                .layerCount     = 1,
	            },
	    }});

	commandBuffer.GetCommandBuffer(0).end();

	graphicsQueue.submit({{
	    .commandBufferCount = 1,
	    .pCommandBuffers    = &commandBuffer.GetCommandBuffer(0),
	}});
	graphicsQueue.waitIdle();
}

}  // namespace dubu::gfx