#pragma once

namespace dubu::gfx {

class DescriptorPool {
public:
	struct CreateInfo {
		vk::Device                          device    = {};
		std::vector<vk::DescriptorPoolSize> poolSizes = {};
		uint32_t                            maxSets   = {};
	};

public:
	DescriptorPool(const CreateInfo& createInfo);

	[[nodiscard]] const vk::DescriptorPool& GetDescriptorPool() const {
		return *mDescriptorPool;
	}

private:
	vk::UniqueDescriptorPool mDescriptorPool;
};  // namespace dubu::gfx

}  // namespace dubu::gfx