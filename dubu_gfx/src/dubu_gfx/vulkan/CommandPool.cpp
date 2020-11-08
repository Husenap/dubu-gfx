#include "CommandPool.h"

namespace dubu::gfx {

CommandPool::CommandPool(const CreateInfo& createInfo) {
	mCommandPool =
	    createInfo.device.createCommandPoolUnique(vk::CommandPoolCreateInfo{
	        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
	        .queueFamilyIndex = *createInfo.queueFamilies.graphicsFamily,
	    });
}

}  // namespace dubu::gfx