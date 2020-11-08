#include "Framebuffer.h"

namespace dubu::gfx {

Framebuffer::Framebuffer(const CreateInfo& createInfo) {
	mUniqueFramebuffers.resize(createInfo.imageViews.size());

	for (std::size_t i = 0; i < createInfo.imageViews.size(); ++i) {
		vk::ImageView attachments[] = {createInfo.imageViews[i]};

		mUniqueFramebuffers[i] =
		    createInfo.device.createFramebufferUnique(vk::FramebufferCreateInfo{
		        .renderPass      = createInfo.renderPass,
		        .attachmentCount = 1,
		        .pAttachments    = attachments,
		        .width           = createInfo.extent.width,
		        .height          = createInfo.extent.height,
		        .layers          = 1,
		    });
		mFramebuffers.push_back(*mUniqueFramebuffers[i]);
	}
}

}  // namespace dubu::gfx