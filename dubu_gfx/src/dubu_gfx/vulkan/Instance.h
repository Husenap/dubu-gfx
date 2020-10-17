#pragma once

namespace dubu::gfx {

class Instance {
public:
	struct CreateInfo {
		const char*              applicationName    = "application";
		uint32_t                 applicationVersion = 0;
		const char*              engineName         = "engine";
		uint32_t                 engineVersion      = 0;
		std::vector<const char*> requiredExtensions;
		std::vector<const char*> optionalExtensions;
		std::vector<const char*> requiredLayers;
		std::vector<const char*> optionalLayers;
	};

public:
	Instance(const CreateInfo& createInfo);

	vk::UniqueInstance& GetInstance() { return mInstance; }

private:
	vk::UniqueInstance               mInstance;
	vk::UniqueDebugUtilsMessengerEXT mDebugMessenger;
};

}  // namespace dubu::gfx