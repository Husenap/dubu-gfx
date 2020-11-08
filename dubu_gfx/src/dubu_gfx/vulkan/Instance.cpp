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
		DUBU_LOG_ERROR("Validation Layer: {}", callbackData->pMessage);
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

	DUBU_LOG_INFO("Vulkan header version: {}.{}.{}",
	              VK_VERSION_MAJOR(VK_HEADER_VERSION_COMPLETE),
	              VK_VERSION_MINOR(VK_HEADER_VERSION_COMPLETE),
	              VK_VERSION_PATCH(VK_HEADER_VERSION_COMPLETE));

	DUBU_LOG_INFO("Supported Extensions:");
	for (auto& extension : AvailableExtensions) {
		DUBU_LOG_INFO(
		    "\t{}: {}", extension.extensionName, extension.specVersion);
	}

	DUBU_LOG_INFO("Supported Layers: ");
	for (auto& layer : AvailableLayers) {
		DUBU_LOG_INFO("\t{}({}): {}.{}.{}.{}",
		              layer.layerName,
		              layer.description,
		              VK_VERSION_MAJOR(layer.specVersion),
		              VK_VERSION_MINOR(layer.specVersion),
		              VK_VERSION_PATCH(layer.specVersion),
		              layer.implementationVersion);
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

	DUBU_LOG_INFO(
	    "==================================================================");
	DUBU_LOG_INFO("Requested Extensions: ");
	for (auto& extension : requestedExtensions) {
		DUBU_LOG_INFO("\t{}", extension);
	}
	DUBU_LOG_INFO("Requested Layers: ");
	for (auto& layer : requestedLayers) {
		DUBU_LOG_INFO("\t{}", layer);
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
