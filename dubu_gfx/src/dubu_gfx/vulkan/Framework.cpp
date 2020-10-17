#include "Framework.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace dubu::gfx {

static VKAPI_ATTR VkBool32 VKAPI_CALL
DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT,
              const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
              void*) {
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		std::cerr << "Validation Layer: " << callbackData->pMessage
		          << std::endl;
	}

	return VK_FALSE;
}

Framework::Framework(const FrameworkCreateInfo& ci)
    : mCreateInfo(ci) {
	CreateInstance();
	PickPhysicalDevice();
	CreateLogicalDevice();
}

void Framework::CreateInstance() {
	::vk::DynamicLoader dl;
	auto                vkGetInstanceProcAddr =
	    dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

	const auto AvailableExtensions =
	    ::vk::enumerateInstanceExtensionProperties();
	const auto AvailableLayers = ::vk::enumerateInstanceLayerProperties();

	std::cout << "Vulkan header version: "
	          << VK_VERSION_MAJOR(VK_HEADER_VERSION_COMPLETE) << "."
	          << VK_VERSION_MINOR(VK_HEADER_VERSION_COMPLETE) << "."
	          << VK_VERSION_PATCH(VK_HEADER_VERSION_COMPLETE) << std::endl;

	std::cout << "Supported Extensions: " << std::endl;
	for (auto& extension : AvailableExtensions) {
		std::cout << "\t" << extension.extensionName << ": "
		          << extension.specVersion << std::endl;
	}

	std::cout << "Supported Layers: " << std::endl;
	for (auto& layer : AvailableLayers) {
		std::cout << "\t" << layer.layerName << "(" << layer.description
		          << "): " << VK_VERSION_MAJOR(layer.specVersion) << "."
		          << VK_VERSION_MINOR(layer.specVersion) << "."
		          << VK_VERSION_PATCH(layer.specVersion) << "."
		          << layer.implementationVersion << std::endl;
	}

	std::vector<const char*> requestedExtensions =
	    mCreateInfo.requiredExtensions;
	std::copy_if(
	    std::begin(mCreateInfo.optionalExtensions),
	    std::end(mCreateInfo.optionalExtensions),
	    std::back_inserter(requestedExtensions),
	    [&](const char* requestedExtension) {
		    return std::any_of(
		        std::begin(AvailableExtensions),
		        std::end(AvailableExtensions),
		        [&](const auto& availableExtension) {
			        return std::string_view(availableExtension.extensionName) ==
			               requestedExtension;
		        });
	    });

	std::vector<const char*> requestedLayers = mCreateInfo.requiredLayers;
	std::copy_if(
	    std::begin(mCreateInfo.optionalLayers),
	    std::end(mCreateInfo.optionalLayers),
	    std::back_inserter(requestedLayers),
	    [&](const char* requestedLayer) {
		    return std::any_of(
		        std::begin(AvailableLayers),
		        std::end(AvailableLayers),
		        [&](const auto& availableLayer) {
			        return std::string_view(availableLayer.layerName) ==
			               requestedLayer;
		        });
	    });

	std::cout
	    << "=================================================================="
	    << std::endl;
	std::cout << "Requested Extensions: " << std::endl;
	for (auto& extension : requestedExtensions) {
		std::cout << "\t" << extension << std::endl;
	}
	std::cout << "Requested Layers: " << std::endl;
	for (auto& layer : requestedLayers) {
		std::cout << "\t" << layer << std::endl;
	}

	::vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{
	    .messageSeverity =
	        ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
	        ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
	        ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
	    .messageType = ::vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
	                   ::vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
	                   ::vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
	    .pfnUserCallback = DebugCallback,
	};

	::vk::ApplicationInfo applicationInfo{
	    .pApplicationName   = mCreateInfo.applicationName.c_str(),
	    .applicationVersion = mCreateInfo.applicationVersion,
	    .pEngineName        = mCreateInfo.engineName.c_str(),
	    .engineVersion      = mCreateInfo.engineVersion,
	    .apiVersion         = VK_API_VERSION_1_2,
	};

	::vk::InstanceCreateInfo instanceCreateInfo{
	    .pNext               = &debugCreateInfo,
	    .pApplicationInfo    = &applicationInfo,
	    .enabledLayerCount   = static_cast<uint32_t>(requestedLayers.size()),
	    .ppEnabledLayerNames = requestedLayers.data(),
	    .enabledExtensionCount =
	        static_cast<uint32_t>(requestedExtensions.size()),
	    .ppEnabledExtensionNames = requestedExtensions.data(),
	};

	mInstance = ::vk::createInstanceUnique(instanceCreateInfo);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(mInstance.get());

	mDebugMessenger =
	    mInstance->createDebugUtilsMessengerEXTUnique(debugCreateInfo);
}

void Framework::PickPhysicalDevice() {
	const auto AvailableDevices = mInstance->enumeratePhysicalDevices();

	std::cout << "Available Physical Devices:" << std::endl;
	for (auto& device : AvailableDevices) {
		auto properties = device.getProperties();

		std::cout << "\t"
		          << "[" << ::vk::to_string(properties.deviceType) << "]"
		          << properties.deviceName
		          << ": apiVersion:" << VK_VERSION_MAJOR(properties.apiVersion)
		          << "." << VK_VERSION_MINOR(properties.apiVersion) << "."
		          << VK_VERSION_PATCH(properties.apiVersion) << std::endl;
	}

	if (AvailableDevices.empty()) {
		throw std::runtime_error("Couldn't find any physical devices!");
	}

	std::multimap<int, ::vk::PhysicalDevice> candidates;

	const auto ScoreCalculation = [](const ::vk::PhysicalDevice& device) {
		int score = 0;

		const auto                   properties = device.getProperties();
		const auto                   features   = device.getFeatures();
		internal::QueueFamilyIndices queueFamilies(device);

		if (!queueFamilies.IsComplete()) {
			return 0;
		}

		if (!features.geometryShader) {
			return 0;
		}

		if (properties.deviceType == ::vk::PhysicalDeviceType::eDiscreteGpu) {
			score += 10000;
		}

		score += properties.limits.maxImageDimension2D;

		return score;
	};

	for (auto& device : AvailableDevices) {
		candidates.insert(std::make_pair(ScoreCalculation(device), device));
	}

	mPhysicalDevice = candidates.rbegin()->second;
}

void Framework::CreateLogicalDevice() {
	internal::QueueFamilyIndices queueFamilies(mPhysicalDevice);

	float queuePriority = 1.0f;

	::vk::DeviceQueueCreateInfo queueCreateInfo{
	    .queueFamilyIndex = *queueFamilies.graphicsFamily,
	    .queueCount       = 1,
	    .pQueuePriorities = &queuePriority,
	};

	::vk::PhysicalDeviceFeatures deviceFeatures{};

	::vk::DeviceCreateInfo deviceCreateInfo{
	    .queueCreateInfoCount  = 1,
	    .pQueueCreateInfos     = &queueCreateInfo,
	    .enabledLayerCount     = 0,
	    .enabledExtensionCount = 0,
	    .pEnabledFeatures      = &deviceFeatures,
	};

	mDevice = mPhysicalDevice.createDeviceUnique(deviceCreateInfo);

	mGraphicsQueue = mDevice->getQueue(*queueFamilies.graphicsFamily, 0);
}

}  // namespace dubu::gfx