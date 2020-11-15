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

	bool foundDepthAttachment = false;

	std::vector<std::vector<vk::AttachmentReference>> colorAttachmentReferences;
	std::vector<std::optional<vk::AttachmentReference>>
	    depthAttachmentReference;
	for (std::size_t i = 0; i < ci.subpasses.size(); ++i) {
		colorAttachmentReferences.emplace_back();
		depthAttachmentReference.emplace_back();

		const auto& subpassDescription = ci.subpasses[i];

		for (auto attachmentIndex : subpassDescription.colorAttachmentIndices) {
			colorAttachmentReferences[i].push_back(vk::AttachmentReference{
			    .attachment = static_cast<uint32_t>(attachmentIndex),
			    .layout     = ci.attachments[attachmentIndex].referenceLayout,
			});
		}

		if (subpassDescription.depthAttachmentIndex) {
			depthAttachmentReference[i] = vk::AttachmentReference{
			    .attachment = static_cast<uint32_t>(
			        *subpassDescription.depthAttachmentIndex),
			    .layout =
			        ci.attachments[*subpassDescription.depthAttachmentIndex]
			            .referenceLayout,
			};
			foundDepthAttachment = true;
		}
	}

	std::vector<vk::SubpassDescription> subpasses;
	for (std::size_t i = 0; i < ci.subpasses.size(); ++i) {
		const auto& subpassDescription = ci.subpasses[i];
		subpasses.push_back(vk::SubpassDescription{
		    .pipelineBindPoint = subpassDescription.bindPoint,
		    .colorAttachmentCount =
		        static_cast<uint32_t>(colorAttachmentReferences[i].size()),
		    .pColorAttachments       = colorAttachmentReferences[i].data(),
		    .pDepthStencilAttachment = depthAttachmentReference[i]
		                                   ? &*depthAttachmentReference[i]
		                                   : nullptr,
		});
	}

	vk::PipelineStageFlags sourceStageMask =
	    vk::PipelineStageFlagBits::eColorAttachmentOutput;
	vk::PipelineStageFlags destinationStageMask =
	    vk::PipelineStageFlagBits::eColorAttachmentOutput;
	vk::AccessFlags destinationAccessMask =
	    vk::AccessFlagBits::eColorAttachmentWrite;

	if (foundDepthAttachment) {
		sourceStageMask |= vk::PipelineStageFlagBits::eEarlyFragmentTests;
		destinationStageMask |= vk::PipelineStageFlagBits::eEarlyFragmentTests;
		destinationAccessMask |=
		    vk::AccessFlagBits::eDepthStencilAttachmentWrite;
	}

	vk::SubpassDependency dependency{
	    .srcSubpass    = VK_SUBPASS_EXTERNAL,
	    .dstSubpass    = 0,
	    .srcStageMask  = sourceStageMask,
	    .dstStageMask  = destinationStageMask,
	    .dstAccessMask = destinationAccessMask,
	};

	mRenderPass = ci.device.createRenderPassUnique(vk::RenderPassCreateInfo{
	    .attachmentCount = static_cast<uint32_t>(attachments.size()),
	    .pAttachments    = attachments.data(),
	    .subpassCount    = static_cast<uint32_t>(subpasses.size()),
	    .pSubpasses      = subpasses.data(),
	    .dependencyCount = 1,
	    .pDependencies   = &dependency,
	});
}

}  // namespace dubu::gfx