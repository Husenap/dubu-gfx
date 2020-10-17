#include "QueueFamilyIndices.h"

namespace dubu::gfx::internal {

QueueFamilyIndices::QueueFamilyIndices(::vk::PhysicalDevice device) {
	int i = 0;
	for (const auto& queueFamily : device.getQueueFamilyProperties()) {
		if (IsComplete()) {
			break;
		}

		if (queueFamily.queueFlags & ::vk::QueueFlagBits::eGraphics) {
			graphicsFamily = i;
		}

		++i;
	}
}

}  // namespace dubu::gfx::internal