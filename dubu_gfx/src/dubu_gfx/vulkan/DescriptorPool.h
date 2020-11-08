#pragma once

namespace dubu::gfx {

class DescriptorPool {
public:
	struct CreateInfo {
		vk::Device                          device    = {};
		std::vector<vk::DescriptorPoolSize> poolSizes = {};
		uint32_t                            maxSets   = {};
	};

	const vk::DescriptorPool& GetDescriptorPool() const {
		return *mDescriptorPool;
	}

public:
	DescriptorPool(const CreateInfo& createInfo);

private:
	vk::UniqueDescriptorPool mDescriptorPool;
};  // namespace dubu::gfx

}  // namespace dubu::gfx