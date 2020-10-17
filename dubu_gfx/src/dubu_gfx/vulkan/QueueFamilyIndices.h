#pragma once

namespace dubu::gfx::internal {
struct QueueFamilyIndices {
	QueueFamilyIndices(vk::PhysicalDevice device);

	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool IsComplete() const {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};
}  // namespace dubu::gfx::internal
