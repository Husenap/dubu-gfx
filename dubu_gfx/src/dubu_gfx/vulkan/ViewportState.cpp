#include "ViewportState.h"

namespace dubu::gfx {

ViewportState::ViewportState(const CreateInfo& ci)
    : mCreateInfo(ci) {}

vk::PipelineViewportStateCreateInfo ViewportState::GetViewportState() const {
	return {
	    .viewportCount = static_cast<uint32_t>(mCreateInfo.viewports.size()),
	    .pViewports    = mCreateInfo.viewports.data(),
	    .scissorCount  = static_cast<uint32_t>(mCreateInfo.scissors.size()),
	    .pScissors     = mCreateInfo.scissors.data(),
	};
}

}  // namespace dubu::gfx