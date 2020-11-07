#pragma once

namespace dubu::gfx {

class ViewportState {
public:
	struct CreateInfo {
		std::vector<vk::Viewport> viewports = {};
		std::vector<vk::Rect2D>   scissors  = {};
	};

public:
	ViewportState(const CreateInfo& ci);

	vk::PipelineViewportStateCreateInfo GetViewportState() const;
	const std::vector<vk::Viewport>&    GetViewports() const {
        return mCreateInfo.viewports;
	}

private:
	CreateInfo mCreateInfo;
};

}  // namespace dubu::gfx