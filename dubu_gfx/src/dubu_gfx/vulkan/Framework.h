#pragma once

#include <vulkan/vulkan.hpp>

#include "QueueFamilyIndices.h"

namespace dubu::gfx {

struct FrameworkCreateInfo {
	std::string              applicationName    = "application";
	uint32_t                 applicationVersion = 0;
	std::string              engineName         = "engine";
	uint32_t                 engineVersion      = 0;
	std::vector<const char*> requiredExtensions;
	std::vector<const char*> optionalExtensions;
	std::vector<const char*> requiredLayers;
	std::vector<const char*> optionalLayers;
};

class Framework {
public:
	Framework(const FrameworkCreateInfo& ci);

private:
	void CreateInstance();
	void PickPhysicalDevice();
	void CreateLogicalDevice();

	FrameworkCreateInfo                mCreateInfo;
	::vk::UniqueInstance               mInstance;
	::vk::UniqueDebugUtilsMessengerEXT mDebugMessenger;
	::vk::PhysicalDevice               mPhysicalDevice;
	::vk::UniqueDevice                 mDevice;
	::vk::Queue                        mGraphicsQueue;
};
}  // namespace dubu::gfx