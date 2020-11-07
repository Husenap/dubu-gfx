#include "Swapchain.h"

#include "QueueFamilyIndices.h"

namespace dubu::gfx {
Swapchain::Swapchain(const CreateInfo& createInfo) {
	CreateSwapchain(createInfo.device,
	                createInfo.physicalDevice,
	                createInfo.surface,
	                createInfo.extent);
}

vk::SurfaceFormatKHR Swapchain::ChooseSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR>& formats) const {
	for (auto format : formats) {
		if (format.format == vk::Format::eB8G8R8A8Srgb &&
		    format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			return format;
		}
	}

	return formats[0];
}

vk::PresentModeKHR Swapchain::ChoosePresentMode(
    const std::vector<vk::PresentModeKHR>& presentModes) const {
	for (auto presentMode : presentModes) {
		if (presentMode == vk::PresentModeKHR::eMailbox) {
			return presentMode;
		}
	}

	return vk::PresentModeKHR::eFifo;
}
vk::Extent2D Swapchain::ChooseExtent(
    const vk::SurfaceCapabilitiesKHR& capabilities, vk::Extent2D extent) const {
	if (capabilities.currentExtent.width !=
	    std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	} else {
		vk::Extent2D actualExtent = extent;

		actualExtent.width = std::max(
		    capabilities.minImageExtent.width,
		    std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(
		    capabilities.minImageExtent.height,
		    std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}
void Swapchain::CreateSwapchain(vk::Device         device,
                                vk::PhysicalDevice physicalDevice,
                                vk::SurfaceKHR     surface,
                                vk::Extent2D       extent) {
	const auto Capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	const auto PresentMode =
	    ChoosePresentMode(physicalDevice.getSurfacePresentModesKHR(surface));
	const auto Format =
	    ChooseSurfaceFormat(physicalDevice.getSurfaceFormatsKHR(surface));

	mImageFormat = Format.format;
	mExtent      = ChooseExtent(Capabilities, extent);

	internal::QueueFamilyIndices queueFamilies(physicalDevice, surface);

	uint32_t imageCount = Capabilities.minImageCount + 1;
	if (Capabilities.maxImageCount > 0) {
		imageCount = std::min(imageCount, Capabilities.maxImageCount);
	}

	vk::SharingMode sharingMode          = vk::SharingMode::eExclusive;
	uint32_t        familyIndexCount     = 0;
	uint32_t        queueFamilyIndices[] = {*queueFamilies.graphicsFamily,
                                     *queueFamilies.presentFamily};
	if (queueFamilies.graphicsFamily != queueFamilies.presentFamily) {
		sharingMode      = vk::SharingMode::eConcurrent;
		familyIndexCount = 2;
	}

	mSwapchain = device.createSwapchainKHRUnique(vk::SwapchainCreateInfoKHR{
	    .surface               = surface,
	    .minImageCount         = imageCount,
	    .imageFormat           = mImageFormat,
	    .imageColorSpace       = Format.colorSpace,
	    .imageExtent           = mExtent,
	    .imageArrayLayers      = 1,
	    .imageUsage            = vk::ImageUsageFlagBits::eColorAttachment,
	    .imageSharingMode      = sharingMode,
	    .queueFamilyIndexCount = familyIndexCount,
	    .pQueueFamilyIndices   = queueFamilyIndices,
	    .preTransform          = Capabilities.currentTransform,
	    .compositeAlpha        = vk::CompositeAlphaFlagBitsKHR::eOpaque,
	    .presentMode           = PresentMode,
	    .clipped               = VK_TRUE,
	    .oldSwapchain          = nullptr,
	});

	mImages = device.getSwapchainImagesKHR(*mSwapchain);

	for (std::size_t i = 0; i < mImages.size(); ++i) {
		mUniqueImageViews.push_back(
		    device.createImageViewUnique(vk::ImageViewCreateInfo{
		        .image    = mImages[i],
		        .viewType = vk::ImageViewType::e2D,
		        .format   = mImageFormat,
		        .components =
		            {
		                .r = vk::ComponentSwizzle::eIdentity,
		                .g = vk::ComponentSwizzle::eIdentity,
		                .b = vk::ComponentSwizzle::eIdentity,
		                .a = vk::ComponentSwizzle::eIdentity,
		            },
		        .subresourceRange =
		            {
		                .aspectMask     = vk::ImageAspectFlagBits::eColor,
		                .baseMipLevel   = 0,
		                .levelCount     = 1,
		                .baseArrayLayer = 0,
		                .layerCount     = 1,
		            },
		    }));
		mImageViews.push_back(*mUniqueImageViews[i]);
	}
}
}  // namespace dubu::gfx