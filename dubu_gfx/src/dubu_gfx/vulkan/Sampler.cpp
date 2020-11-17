#include "Sampler.h"

namespace dubu::gfx {

Sampler::Sampler(const CreateInfo& createInfo) {
	mSampler = createInfo.device.createSamplerUnique(vk::SamplerCreateInfo{
	    .magFilter               = createInfo.filter,
	    .minFilter               = createInfo.filter,
	    .mipmapMode              = vk::SamplerMipmapMode::eLinear,
	    .addressModeU            = createInfo.addressMode,
	    .addressModeV            = createInfo.addressMode,
	    .addressModeW            = createInfo.addressMode,
	    .mipLodBias              = 0.f,
	    .anisotropyEnable        = VK_TRUE,
	    .maxAnisotropy           = 16.0f,
	    .compareEnable           = VK_FALSE,
	    .compareOp               = vk::CompareOp::eAlways,
	    .minLod                  = 0.f,
	    .maxLod                  = VK_LOD_CLAMP_NONE,
	    .borderColor             = vk::BorderColor::eIntOpaqueBlack,
	    .unnormalizedCoordinates = VK_FALSE,
	});
}

}  // namespace dubu::gfx