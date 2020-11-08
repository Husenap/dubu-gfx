#pragma once

namespace dubu::gfx {
class Device;
class Surface;
}  // namespace dubu::gfx

namespace dubu::gfx::internal {
struct QueueFamilyIndices {
	QueueFamilyIndices(vk::PhysicalDevice physicalDevice,
	                   vk::SurfaceKHR     surface);

	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool IsComplete() const {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};
}  // namespace dubu::gfx::internal
