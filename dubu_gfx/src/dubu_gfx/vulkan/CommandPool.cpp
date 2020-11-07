#include "CommandPool.h"

#include "QueueFamilyIndices.h"

namespace dubu::gfx {

CommandPool::CommandPool(const CreateInfo& createInfo) {
	internal::QueueFamilyIndices queueFamilies(createInfo.physicalDevice,
	                                           createInfo.surface);

	mCommandPool =
	    createInfo.device.createCommandPoolUnique(vk::CommandPoolCreateInfo{
	        .queueFamilyIndex = *queueFamilies.graphicsFamily});
}

}  // namespace dubu::gfx