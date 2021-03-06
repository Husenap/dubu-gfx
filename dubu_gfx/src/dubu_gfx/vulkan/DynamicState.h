#pragma once

namespace dubu::gfx {

class DynamicState {
public:
	DynamicState(const std::vector<vk::DynamicState>& dynamicStates);

	[[nodiscard]] vk::PipelineDynamicStateCreateInfo GetDynamicState() const;

private:
	std::vector<vk::DynamicState> mDynamicStates;
};

}  // namespace dubu::gfx