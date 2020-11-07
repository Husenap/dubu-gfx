#pragma once

namespace dubu::gfx {

class Swapchain {
public:
	struct CreateInfo {
		vk::Device         device         = {};
		vk::PhysicalDevice physicalDevice = {};
		vk::SurfaceKHR     surface        = {};
		vk::Extent2D       extent         = {};
	};

public:
	Swapchain(const CreateInfo& createInfo);

	vk::Format                 GetImageFormat() const { return mImageFormat; }
	const vk::Extent2D&        GetExtent() const { return mExtent; }
	std::vector<vk::ImageView> GetImageViews() const { return mImageViews; }
	const vk::SwapchainKHR&    GetSwapchain() const { return *mSwapchain; }

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
	std::vector<vk::Image>           mImages;
	std::vector<vk::UniqueImageView> mUniqueImageViews;
	std::vector<vk::ImageView>       mImageViews;
};

}  // namespace dubu::gfx