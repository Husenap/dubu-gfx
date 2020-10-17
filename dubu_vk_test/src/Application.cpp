#include "Application.h"

#include <iostream>

void Application::Run() {
	mWindow = std::make_unique<dubu::window::GLFWWindow>(600, 600, "dubu-vk");

	InitVulkan();
	MainLoop();
}

void Application::InitVulkan() {
	std::cout << "Supported Extensions: " << std::endl;
	for (auto& extension : vk::enumerateInstanceExtensionProperties()) {
		std::cout << "\t" << extension.extensionName << ": "
		          << extension.specVersion << std::endl;
	}

	std::cout << "Supported Layers: " << std::endl;
	for (auto& layer : vk::enumerateInstanceLayerProperties()) {
		std::cout << "\t" << layer.layerName << "(" << layer.description
		          << "): " << layer.implementationVersion << "."
		          << layer.specVersion << std::endl;
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
