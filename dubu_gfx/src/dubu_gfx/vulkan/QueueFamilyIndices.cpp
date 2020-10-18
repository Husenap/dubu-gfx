#include "QueueFamilyIndices.h"

#include "Device.h"
#include "Surface.h"

namespace dubu::gfx::internal {

QueueFamilyIndices::QueueFamilyIndices(vk::PhysicalDevice physicalDevice,
                                       vk::SurfaceKHR     surface) {
	int i = 0;
	for (const auto& queueFamily : physicalDevice.getQueueFamilyProperties()) {
		if (IsComplete()) {
			break;
		}

		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			graphicsFamily = i;
		}

		if (physicalDevice.getSurfaceSupportKHR(i, surface)) {
			presentFamily = i;
		}

		++i;
	}
}

}  // namespace dubu::gfx::internal