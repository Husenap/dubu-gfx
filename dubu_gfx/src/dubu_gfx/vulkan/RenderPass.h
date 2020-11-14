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
	};

	struct CreateInfo {
		vk::Device                         device      = {};
		std::vector<AttachmentDescription> attachments = {};
		std::vector<std::tuple<vk::PipelineBindPoint, std::vector<std::size_t>>>
		    subpasses = {};
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