#include "Buffer.h"

#include "CommandBuffer.h"
#include "CommandPool.h"

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

	mMemory = std::make_unique<DeviceMemory>(DeviceMemory::CreateInfo{
	    .device           = createInfo.device,
	    .physicalDevice   = createInfo.physicalDevice,
	    .allocationSize   = memRequirements.size,
	    .typeFilter       = memRequirements.memoryTypeBits,
	    .memoryProperties = createInfo.memoryProperties,
	});

	createInfo.device.bindBufferMemory(*mBuffer, mMemory->GetDeviceMemory(), 0);
}

void Buffer::SetData(const void* bytes, std::size_t numBytes) {
	void* data = mMemory->Map(numBytes);

	std::memcpy(data, bytes, numBytes);

	mMemory->Unmap();
}

void Buffer::SetData(const vk::Buffer&         buffer,
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

	commandBuffer.GetCommandBuffer(0).copyBuffer(buffer,
	                                             *mBuffer,
	                                             {vk::BufferCopy{
	                                                 .size = mCreateInfo.size,
	                                             }});

	commandBuffer.GetCommandBuffer(0).end();

	graphicsQueue.submit({{
	    .commandBufferCount = 1,
	    .pCommandBuffers    = &commandBuffer.GetCommandBuffer(0),
	}});
	graphicsQueue.waitIdle();
}

}  // namespace dubu::gfx