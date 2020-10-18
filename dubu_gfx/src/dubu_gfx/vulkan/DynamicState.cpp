#include "DynamicState.h"

namespace dubu::gfx {

DynamicState::DynamicState(const std::vector<vk::DynamicState>& dynamicStates)
    : mDynamicStates(dynamicStates) {}

vk::PipelineDynamicStateCreateInfo DynamicState::GetDynamicState() const {
	return {
	    .dynamicStateCount = static_cast<uint32_t>(mDynamicStates.size()),
	    .pDynamicStates    = mDynamicStates.data(),
	};
}

}  // namespace dubu::gfx