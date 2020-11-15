#pragma once

#include "QueueFamilyIndices.h"

namespace dubu::gfx {

class Swapchain {
public:
	struct CreateInfo {
		vk::Device         device         = {};
		vk::PhysicalDevice physicalDevice = {};
		vk::SurfaceKHR     surface        = {};
		QueueFamilyIndices queueFamilies  = {};
		vk::Extent2D       extent         = {};
	};

public:
	Swapchain(const CreateInfo& createInfo);

	[[nodiscard]] vk::Format GetImageFormat() const { return mImageFormat; }
	[[nodiscard]] const vk::Extent2D& GetExtent() const { return mExtent; }
	[[nodiscard]] std::size_t         GetImageCount() const {
        return mImageViews.size();
	}
	[[nodiscard]] const std::vector<vk::ImageView>& GetImageViews() const {
		return mImageViews;
	}
	[[nodiscard]] const vk::SwapchainKHR& GetSwapchain() const {
		return *mSwapchain;
	}

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
	                     QueueFamilyIndices queueFamilies,
	                     vk::Extent2D       extent);

	vk::Format   mImageFormat;
	vk::Extent2D mExtent;

	vk::UniqueSwapchainKHR           mSwapchain;
	std::vector<vk::Image>           mImages;
	std::vector<vk::UniqueImageView> mUniqueImageViews;
	std::vector<vk::ImageView>       mImageViews;
};

}  // namespace dubu::gfx