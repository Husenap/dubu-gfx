#include "Application.h"

#include <iostream>

void Application::Run() {
	mWindow = std::make_unique<dubu::window::GLFWWindow>(600, 600, "dubu-vk");

	InitVulkan();
	MainLoop();
}

void Application::InitVulkan() {
	std::cout << "Vulkan header version: "
	          << VK_VERSION_MAJOR(VK_HEADER_VERSION_COMPLETE) << "."
	          << VK_VERSION_MINOR(VK_HEADER_VERSION_COMPLETE) << "."
	          << VK_VERSION_PATCH(VK_HEADER_VERSION_COMPLETE) << std::endl;

	std::cout << "Supported Extensions: " << std::endl;
	for (auto& extension : vk::enumerateInstanceExtensionProperties()) {
		std::cout << "\t" << extension.extensionName << ": "
		          << extension.specVersion << std::endl;
	}

	std::cout << "Supported Layers: " << std::endl;
	for (auto& layer : vk::enumerateInstanceLayerProperties()) {
		std::cout << "\t" << layer.layerName << "(" << layer.description
		          << "): " << VK_VERSION_MAJOR(layer.specVersion) << "."
		          << VK_VERSION_MINOR(layer.specVersion) << "."
		          << VK_VERSION_PATCH(layer.specVersion) << "."
		          << layer.implementationVersion << std::endl;
	}

	vk::ApplicationInfo applicationInfo{
	    .pApplicationName   = "sample",
	    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
	    .pEngineName        = "dubu-vk",
	    .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
	    .apiVersion         = VK_API_VERSION_1_2};

	uint32_t     extensionCount = 0;
	const char** extensionNames =
	    glfwGetRequiredInstanceExtensions(&extensionCount);

	vk::InstanceCreateInfo instanceCreateInfo{
	    .pApplicationInfo        = &applicationInfo,
	    .enabledExtensionCount   = extensionCount,
	    .ppEnabledExtensionNames = extensionNames,
	};

	mInstance = vk::createInstanceUnique(instanceCreateInfo);
}

void Application::MainLoop() {
	while (!mWindow->ShouldClose()) {
		mWindow->PollEvents();
	}
}
