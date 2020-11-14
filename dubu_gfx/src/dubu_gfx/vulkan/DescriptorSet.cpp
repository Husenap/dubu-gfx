#include "DescriptorSet.h"

namespace dubu::gfx {

DescriptorSet::DescriptorSet(const CreateInfo& createInfo)
    : mCreateInfo(createInfo) {
	mDescriptorSets = createInfo.device.allocateDescriptorSetsUnique(
	    vk::DescriptorSetAllocateInfo{
	        .descriptorPool     = createInfo.descriptorPool,
	        .descriptorSetCount = createInfo.descriptorSetCount,
	        .pSetLayouts        = createInfo.layouts.data(),
	    });
}

void DescriptorSet::UpdateDescriptorSets(
    std::size_t index, std::vector<vk::WriteDescriptorSet>& descriptorWrites) {
	for (auto& descriptorWrite : descriptorWrites) {
		descriptorWrite.dstSet = *mDescriptorSets[index];
	}

	mCreateInfo.device.updateDescriptorSets(descriptorWrites, {});
}

}  // namespace dubu::gfx