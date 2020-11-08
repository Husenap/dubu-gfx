#include "Instance.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace dubu::gfx {

#ifdef _DEBUG
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
#endif

Instance::Instance(const CreateInfo& createInfo) {
	vk::DynamicLoader dl;
	auto              vkGetInstanceProcAddr =
	    dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

	const auto AvailableExtensions = vk::enumerateInstanceExtensionProperties();
	const auto AvailableLayers     = vk::enumerateInstanceLayerProperties();

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
	    createInfo.requiredExtensions;
	std::copy_if(
	    std::begin(createInfo.optionalExtensions),
	    std::end(createInfo.optionalExtensions),
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

	std::vector<const char*> requestedLayers = createInfo.requiredLayers;
	std::copy_if(
	    std::begin(createInfo.optionalLayers),
	    std::end(createInfo.optionalLayers),
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

#ifdef _DEBUG
	requestedExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	requestedLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

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

#ifdef _DEBUG
	vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{
	    .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
	                       vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
	                       vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
	    .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
	                   vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
	                   vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
	    .pfnUserCallback = DebugCallback,
	};
#endif

	vk::ApplicationInfo applicationInfo{
	    .pApplicationName   = createInfo.applicationName,
	    .applicationVersion = createInfo.applicationVersion,
	    .pEngineName        = createInfo.engineName,
	    .engineVersion      = createInfo.engineVersion,
	    .apiVersion         = VK_API_VERSION_1_2,
	};

	vk::InstanceCreateInfo instanceCreateInfo{
#ifdef _DEBUG
	    .pNext = &debugCreateInfo,
#endif
	    .pApplicationInfo    = &applicationInfo,
	    .enabledLayerCount   = static_cast<uint32_t>(requestedLayers.size()),
	    .ppEnabledLayerNames = requestedLayers.data(),
	    .enabledExtensionCount =
	        static_cast<uint32_t>(requestedExtensions.size()),
	    .ppEnabledExtensionNames = requestedExtensions.data(),
	};

	mInstance = vk::createInstanceUnique(instanceCreateInfo);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(mInstance.get());

#ifdef _DEBUG
	mDebugMessenger =
	    mInstance->createDebugUtilsMessengerEXTUnique(debugCreateInfo);
#endif
}

}  // namespace dubu::gfx
