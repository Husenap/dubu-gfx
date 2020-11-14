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
    const std::vector<vk::Buffer>& buffers) {
	for (std::size_t i = 0; i < mDescriptorSets.size(); ++i) {
		vk::DescriptorBufferInfo bufferInfo{
		    .buffer = buffers[i],
		    .offset = 0,
		    .range  = VK_WHOLE_SIZE,
		};
		mCreateInfo.device.updateDescriptorSets(
		    {
		        vk::WriteDescriptorSet{
		            .dstSet          = *mDescriptorSets[i],
		            .dstBinding      = 0,
		            .dstArrayElement = 0,
		            .descriptorCount = 1,
		            .descriptorType  = vk::DescriptorType::eUniformBuffer,
		            .pBufferInfo     = &bufferInfo,
		        },
		    },
		    {});
	}
}

}  // namespace dubu::gfx