#pragma once

namespace dubu::gfx {

class DescriptorSet {
public:
	struct CreateInfo {
		vk::Device                           device             = {};
		vk::DescriptorPool                   descriptorPool     = {};
		uint32_t                             descriptorSetCount = {};
		std::vector<vk::DescriptorSetLayout> layouts            = {};
	};

public:
	DescriptorSet(const CreateInfo& createInfo);

	void UpdateDescriptorSets(
	    std::size_t                          index,
	    std::vector<vk::WriteDescriptorSet>& descriptorWrites);

	[[nodiscard]] const vk::DescriptorSet& GetDescriptorSet(
	    std::size_t index = 0) {
		return *mDescriptorSets[index];
	}

private:
	CreateInfo mCreateInfo;

	std::vector<vk::UniqueDescriptorSet> mDescriptorSets;
};

}  // namespace dubu::gfx