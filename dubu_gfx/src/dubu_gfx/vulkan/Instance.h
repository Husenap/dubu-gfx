#pragma once

namespace dubu::gfx {

class Instance {
public:
	struct CreateInfo {
		const char*              applicationName    = "application";
		uint32_t                 applicationVersion = {};
		const char*              engineName         = "engine";
		uint32_t                 engineVersion      = {};
		std::vector<const char*> requiredExtensions = {};
		std::vector<const char*> optionalExtensions = {};
		std::vector<const char*> requiredLayers     = {};
		std::vector<const char*> optionalLayers     = {};
	};

public:
	Instance(const CreateInfo& createInfo);

	vk::Instance GetInstance() const { return *mInstance; }

private:
	vk::UniqueInstance mInstance;

#ifdef _DEBUG
	vk::UniqueDebugUtilsMessengerEXT mDebugMessenger;
#endif
};

}  // namespace dubu::gfx