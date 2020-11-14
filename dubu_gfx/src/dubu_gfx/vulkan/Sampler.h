#pragma once

namespace dubu::gfx {

class Sampler {
public:
	struct CreateInfo {
		vk::Device             device      = {};
		vk::Filter             filter      = {};
		vk::SamplerAddressMode addressMode = {};
	};

public:
	Sampler(const CreateInfo& createInfo);

	[[nodiscard]] const vk::Sampler& GetSampler() const { return *mSampler; }

private:
	vk::UniqueSampler mSampler;
};

}  // namespace dubu::gfx