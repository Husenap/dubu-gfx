#include "QueueFamilyIndices.h"

namespace dubu::gfx {

QueueFamilyIndices::QueueFamilyIndices(vk::PhysicalDevice physicalDevice,
                                       vk::SurfaceKHR     surface) {
	uint32_t i = 0;
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

}  // namespace dubu::gfx