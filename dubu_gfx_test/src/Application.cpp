#include "Application.h"

void Application::Run() {
	mWindow = std::make_unique<dubu::window::GLFWWindow>(600, 600, "dubu-gfx");

	InitFramework();
	MainLoop();
}

void Application::InitFramework() {
	uint32_t     extensionCount = 0;
	const char** extensionNames =
	    glfwGetRequiredInstanceExtensions(&extensionCount);
	std::vector<const char*> requiredExtensions;
	for (uint32_t i = 0; i < extensionCount; ++i) {
		requiredExtensions.push_back(extensionNames[i]);
	}

	mInstance =
	    std::make_unique<dubu::gfx::Instance>(dubu::gfx::Instance::CreateInfo{
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

	VkSurfaceKHR surface;
	if (glfwCreateWindowSurface(*mInstance->GetInstance(),
	                            mWindow->GetGLFWHandle(),
	                            nullptr,
	                            &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to created Surface!");
	}

	mSurface = std::make_unique<dubu::gfx::Surface>(
	    vk::UniqueSurfaceKHR(surface, *mInstance->GetInstance()));

	mDevice = std::make_unique<dubu::gfx::Device>(*mInstance);
}

void Application::MainLoop() {
	while (!mWindow->ShouldClose()) {
		mWindow->PollEvents();
	}
}
