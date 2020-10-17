#include "Device.h"

#include "QueueFamilyIndices.h"

namespace dubu::gfx {

Device::Device(Instance& instance) {
	PickPhysicalDevice(instance);
	CreateLogicalDevice();
}

void Device::PickPhysicalDevice(Instance& instance) {
	const auto AvailableDevices =
	    instance.GetInstance()->enumeratePhysicalDevices();

	std::cout << "Available Physical Devices:" << std::endl;
	for (auto& device : AvailableDevices) {
		auto properties = device.getProperties();

		std::cout << "\t"
		          << "[" << vk::to_string(properties.deviceType) << "]"
		          << properties.deviceName
		          << ": apiVersion:" << VK_VERSION_MAJOR(properties.apiVersion)
		          << "." << VK_VERSION_MINOR(properties.apiVersion) << "."
		          << VK_VERSION_PATCH(properties.apiVersion) << std::endl;
	}

	if (AvailableDevices.empty()) {
		throw std::runtime_error("Couldn't find any physical devices!");
	}

	std::multimap<int, vk::PhysicalDevice> candidates;

	const auto ScoreCalculation = [](const vk::PhysicalDevice& device) {
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

		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
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

void Device::CreateLogicalDevice() {
	internal::QueueFamilyIndices queueFamilies(mPhysicalDevice);

	float queuePriority = 1.0f;

	vk::DeviceQueueCreateInfo queueCreateInfo{
	    .queueFamilyIndex = *queueFamilies.graphicsFamily,
	    .queueCount       = 1,
	    .pQueuePriorities = &queuePriority,
	};

	vk::PhysicalDeviceFeatures deviceFeatures{};

	vk::DeviceCreateInfo deviceCreateInfo{
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