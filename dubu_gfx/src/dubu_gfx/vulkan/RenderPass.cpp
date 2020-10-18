#include "RenderPass.h"

namespace dubu::gfx {

RenderPass::RenderPass(const CreateInfo& ci) {
	std::vector<vk::AttachmentDescription> attachments;
	for (const auto& attachment : ci.attachments) {
		attachments.push_back(vk::AttachmentDescription{
		    .flags          = attachment.flags,
		    .format         = attachment.format,
		    .samples        = attachment.samples,
		    .loadOp         = attachment.loadOp,
		    .storeOp        = attachment.storeOp,
		    .stencilLoadOp  = attachment.stencilLoadOp,
		    .stencilStoreOp = attachment.stencilStoreOp,
		    .initialLayout  = attachment.initialLayout,
		    .finalLayout    = attachment.finalLayout,
		});
	}

	std::vector<std::vector<vk::AttachmentReference>> attachmentReferences;
	for (std::size_t i = 0; i < ci.subpasses.size(); ++i) {
		attachmentReferences.emplace_back();
		const auto& [bindPoint, indices] = ci.subpasses[i];
		for (auto attachmentIndex : indices) {
			attachmentReferences[i].push_back(vk::AttachmentReference{
			    .attachment = static_cast<uint32_t>(attachmentIndex),
			    .layout     = ci.attachments[attachmentIndex].referenceLayout,
			});
		}
	}

	std::vector<vk::SubpassDescription> subpasses;
	for (std::size_t i = 0; i < ci.subpasses.size(); ++i) {
		const auto& [bindPoint, indices] = ci.subpasses[i];
		subpasses.push_back(vk::SubpassDescription{
		    .pipelineBindPoint = bindPoint,
		    .colorAttachmentCount =
		        static_cast<uint32_t>(attachmentReferences[i].size()),
		    .pColorAttachments = attachmentReferences[i].data(),
		});
	}

	mRenderPass = ci.device.createRenderPassUnique(vk::RenderPassCreateInfo{
	    .attachmentCount = static_cast<uint32_t>(attachments.size()),
	    .pAttachments    = attachments.data(),
	    .subpassCount    = static_cast<uint32_t>(subpasses.size()),
	    .pSubpasses      = subpasses.data(),
	});
}

}  // namespace dubu::gfx