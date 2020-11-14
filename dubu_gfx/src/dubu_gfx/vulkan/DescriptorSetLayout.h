#pragma once

namespace dubu::gfx {
class DescriptorSetLayout {
public:
	struct CreateInfo {
		vk::Device                                  device;
		std::vector<vk::DescriptorSetLayoutBinding> bindings;
	};

	[[nodiscard]] const vk::DescriptorSetLayout& GetDescriptorSetLayout()
	    const {
		return *mDescriptorSetLayout;
	}

public:
	DescriptorSetLayout(const CreateInfo& createInfo);

private:
	vk::UniqueDescriptorSetLayout mDescriptorSetLayout;
};
}  // namespace dubu::gfx