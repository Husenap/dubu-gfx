#pragma once

namespace dubu::gfx {

class Swapchain {
public:
	Swapchain(vk::Device         device,
	          vk::PhysicalDevice physicalDevice,
	          vk::SurfaceKHR     surface,
	          vk::Extent2D       extent);

	vk::Format          GetImageFormat() const { return mImageFormat; }
	const vk::Extent2D& GetExtent() const { return mExtent; }

private:
	vk::SurfaceFormatKHR ChooseSurfaceFormat(
	    const std::vector<vk::SurfaceFormatKHR>& formats) const;
	vk::PresentModeKHR ChoosePresentMode(
	    const std::vector<vk::PresentModeKHR>& presentModes) const;
	vk::Extent2D ChooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities,
	                          vk::Extent2D                      extent) const;

	void CreateSwapchain(vk::Device         device,
	                     vk::PhysicalDevice physicalDevice,
	                     vk::SurfaceKHR     surface,
	                     vk::Extent2D       extent);

	vk::Format   mImageFormat;
	vk::Extent2D mExtent;

	vk::UniqueSwapchainKHR           mSwapchain;
	std::vector<vk::Image>           mSwapchainImages;
	std::vector<vk::UniqueImageView> mSwapchainImageViews;
};

}  // namespace dubu::gfx