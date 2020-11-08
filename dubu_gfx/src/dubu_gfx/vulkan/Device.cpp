#include "Device.h"

namespace dubu::gfx {

namespace DeviceInternal {
const std::vector<const char*> RequiredExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

Device::Device(vk::Instance instance, vk::SurfaceKHR surface) {
	PickPhysicalDevice(instance, surface);
	CreateLogicalDevice();
}

void Device::PickPhysicalDevice(vk::Instance instance, vk::SurfaceKHR surface) {
	const auto AvailableDevices = instance.enumeratePhysicalDevices();

	DUBU_LOG_INFO("Available Physical Devices:");
	for (auto& device : AvailableDevices) {
		auto properties = device.getProperties();

		DUBU_LOG_INFO("\t[{}]{}: apiVersion:{}.{}.{}",
		              vk::to_string(properties.deviceType),
		              properties.deviceName,
		              VK_VERSION_MAJOR(properties.apiVersion),
		              VK_VERSION_MINOR(properties.apiVersion),
		              VK_VERSION_PATCH(properties.apiVersion));
	}

	if (AvailableDevices.empty()) {
		DUBU_LOG_FATAL("Couldn't find any physical devices!");
	}

	std::multimap<uint32_t, vk::PhysicalDevice> candidates;

	const auto ScoreCalculation = [surface](const vk::PhysicalDevice& device) {
		uint32_t score = 0;

		const auto         properties = device.getProperties();
		const auto         features   = device.getFeatures();
		QueueFamilyIndices queueFamilies(device, surface);

		if (!queueFamilies.IsComplete()) {
			return 0u;
		}

		if (!features.geometryShader) {
			return 0u;
		}

		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			score += 10000;
		}

		score += properties.limits.maxImageDimension2D;

		std::set<std::string> deviceExtensions(
		    std::begin(DeviceInternal::RequiredExtensions),
		    std::end(DeviceInternal::RequiredExtensions));
		for (auto& extension : device.enumerateDeviceExtensionProperties()) {
			deviceExtensions.erase(extension.extensionName);
		}

		if (!deviceExtensions.empty()) {
			return 0u;
		}

		return score;
	};

	for (auto& device : AvailableDevices) {
		candidates.insert(std::make_pair(ScoreCalculation(device), device));
	}

	mPhysicalDevice = candidates.rbegin()->second;

	mQueueFamilies = QueueFamilyIndices(mPhysicalDevice, surface);
}

void Device::CreateLogicalDevice() {
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {*mQueueFamilies.graphicsFamily,
	                                          *mQueueFamilies.presentFamily};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		queueCreateInfos.push_back(vk::DeviceQueueCreateInfo{
		    .queueFamilyIndex = queueFamily,
		    .queueCount       = 1,
		    .pQueuePriorities = &queuePriority,
		});
	}

	vk::PhysicalDeviceFeatures deviceFeatures{};

	vk::DeviceCreateInfo deviceCreateInfo{
	    .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
	    .pQueueCreateInfos    = queueCreateInfos.data(),
	    .enabledLayerCount    = 0,
	    .enabledExtensionCount =
	        static_cast<uint32_t>(DeviceInternal::RequiredExtensions.size()),
	    .ppEnabledExtensionNames = DeviceInternal::RequiredExtensions.data(),
	    .pEnabledFeatures        = &deviceFeatures,
	};

	mDevice = mPhysicalDevice.createDeviceUnique(deviceCreateInfo);

	mGraphicsQueue = mDevice->getQueue(*mQueueFamilies.graphicsFamily, 0);
	mPresentQueue  = mDevice->getQueue(*mQueueFamilies.presentFamily, 0);
}

}  // namespace dubu::gfx