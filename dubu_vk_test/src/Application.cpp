#include "Application.h"

void Application::Run() {
	mWindow = std::make_unique<dubu::window::GLFWWindow>(600, 600, "dubu-vk");

	InitVulkan();
	MainLoop();
}

void Application::InitVulkan() {
	uint32_t     extensionCount = 0;
	const char** extensionNames =
	    glfwGetRequiredInstanceExtensions(&extensionCount);
	std::vector<const char*> requiredExtensions;
	for (uint32_t i = 0; i < extensionCount; ++i) {
		requiredExtensions.push_back(extensionNames[i]);
	}

	mFramework =
	    std::make_unique<dubu::vk::Framework>(dubu::vk::FrameworkCreateInfo{
	        .applicationName    = "sample app",
	        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
	        .engineName         = "dubu-vk",
	        .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
	        .requiredExtensions = requiredExtensions,
#ifdef _DEBUG
	        .optionalExtensions = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME},
	        .optionalLayers     = {"VK_LAYER_KHRONOS_validation"},
#endif
	    });
}

void Application::MainLoop() {
	while (!mWindow->ShouldClose()) {
		mWindow->PollEvents();
	}
}
