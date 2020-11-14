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

public:
	Framebuffer(const CreateInfo& createInfo);

	[[nodiscard]] uint32_t GetFramebufferCount() const {
		return static_cast<uint32_t>(mUniqueFramebuffers.size());
	}
	[[nodiscard]] const std::vector<vk::Framebuffer>& GetFramebuffers() const {
		return mFramebuffers;
	}

private:
	std::vector<vk::UniqueFramebuffer> mUniqueFramebuffers;
	std::vector<vk::Framebuffer>       mFramebuffers;
};

}  // namespace dubu::gfx