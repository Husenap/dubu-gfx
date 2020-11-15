#include "Framebuffer.h"

namespace dubu::gfx {

Framebuffer::Framebuffer(const CreateInfo& createInfo) {
	mUniqueFramebuffers.resize(createInfo.imageViews.size());

	for (std::size_t i = 0; i < createInfo.imageViews.size(); ++i) {
		std::vector<vk::ImageView> attachments = {createInfo.imageViews[i]};
		if (createInfo.depthImageView) {
			attachments.push_back(*createInfo.depthImageView);
		}

		mUniqueFramebuffers[i] =
		    createInfo.device.createFramebufferUnique(vk::FramebufferCreateInfo{
		        .renderPass      = createInfo.renderPass,
		        .attachmentCount = static_cast<uint32_t>(attachments.size()),
		        .pAttachments    = attachments.data(),
		        .width           = createInfo.extent.width,
		        .height          = createInfo.extent.height,
		        .layers          = 1,
		    });
		mFramebuffers.push_back(*mUniqueFramebuffers[i]);
	}
}

}  // namespace dubu::gfx