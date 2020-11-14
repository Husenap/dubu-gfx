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

	[[nodiscard]] vk::PipelineViewportStateCreateInfo GetViewportState() const;
	[[nodiscard]] const std::vector<vk::Viewport>&    GetViewports() const {
        return mCreateInfo.viewports;
	}

private:
	CreateInfo mCreateInfo;
};

}  // namespace dubu::gfx