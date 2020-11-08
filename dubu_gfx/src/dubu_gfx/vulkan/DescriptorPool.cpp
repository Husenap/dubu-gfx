#include "DescriptorPool.h"

namespace dubu::gfx {

DescriptorPool::DescriptorPool(const CreateInfo& createInfo) {
	mDescriptorPool = createInfo.device.createDescriptorPoolUnique(
	    vk::DescriptorPoolCreateInfo{
	        .maxSets       = createInfo.maxSets,
	        .poolSizeCount = static_cast<uint32_t>(createInfo.poolSizes.size()),
	        .pPoolSizes    = createInfo.poolSizes.data(),
	    });
}

}  // namespace dubu::gfx