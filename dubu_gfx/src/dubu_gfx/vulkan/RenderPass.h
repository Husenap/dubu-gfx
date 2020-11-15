#pragma once

namespace dubu::gfx {

class RenderPass {
public:
	struct AttachmentDescription {
		vk::AttachmentDescriptionFlags flags           = {};
		vk::Format                     format          = {};
		vk::SampleCountFlagBits        samples         = {};
		vk::AttachmentLoadOp           loadOp          = {};
		vk::AttachmentStoreOp          storeOp         = {};
		vk::AttachmentLoadOp           stencilLoadOp   = {};
		vk::AttachmentStoreOp          stencilStoreOp  = {};
		vk::ImageLayout                initialLayout   = {};
		vk::ImageLayout                finalLayout     = {};
		vk::ImageLayout                referenceLayout = {};

		[[nodiscard]] vk::AttachmentDescription ToVulkanStruct() const {
			return vk::AttachmentDescription{
			    .flags          = flags,
			    .format         = format,
			    .samples        = samples,
			    .loadOp         = loadOp,
			    .storeOp        = storeOp,
			    .stencilLoadOp  = stencilLoadOp,
			    .stencilStoreOp = stencilStoreOp,
			    .initialLayout  = initialLayout,
			    .finalLayout    = finalLayout,
			};
		}
	};

	struct SubpassDescription {
		vk::PipelineBindPoint      bindPoint              = {};
		std::vector<std::size_t>   colorAttachmentIndices = {};
		std::optional<std::size_t> depthAttachmentIndex   = {};
	};

	struct CreateInfo {
		vk::Device                         device      = {};
		std::vector<AttachmentDescription> attachments = {};
		std::vector<SubpassDescription>    subpasses   = {};
	};

public:
	RenderPass(const CreateInfo& ci);

	[[nodiscard]] const vk::RenderPass& GetRenderPass() const {
		return *mRenderPass;
	}

private:
	vk::UniqueRenderPass mRenderPass;
};

}  // namespace dubu::gfx