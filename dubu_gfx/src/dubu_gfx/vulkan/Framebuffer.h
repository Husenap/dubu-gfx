#pragma once

namespace dubu::gfx {

class Framebuffer {
public:
	struct CreateInfo {
		vk::Device                 device     = {};
		vk::RenderPass             renderPass = {};
		std::vector<vk::ImageView> imageViews = {};
		vk::Extent2D               extent     = {};
	};

	uint32_t GetFramebufferCount() const {
		return static_cast<uint32_t>(mUniqueFramebuffers.size());
	}
	const std::vector<vk::Framebuffer>& GetFramebuffers() const {
		return mFramebuffers;
	}

public:
	Framebuffer(const CreateInfo& createInfo);

private:
	std::vector<vk::UniqueFramebuffer> mUniqueFramebuffers;
	std::vector<vk::Framebuffer>       mFramebuffers;
};

}  // namespace dubu::gfx