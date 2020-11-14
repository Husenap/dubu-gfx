#include "DescriptorSetLayout.h"

namespace dubu::gfx {

DescriptorSetLayout::DescriptorSetLayout(const CreateInfo& createInfo) {
	mDescriptorSetLayout = createInfo.device.createDescriptorSetLayoutUnique(
	    vk::DescriptorSetLayoutCreateInfo{
	        .bindingCount = static_cast<uint32_t>(createInfo.bindings.size()),
	        .pBindings    = createInfo.bindings.data(),
	    });
}

}  // namespace dubu::gfx