#pragma once

#include "Instance.h"
#include "QueueFamilyIndices.h"

namespace dubu::gfx {

class Device {
public:
	Device(vk::Instance instance, vk::SurfaceKHR surface);

	[[nodiscard]] const vk::PhysicalDevice& GetPhysicalDevice() const {
		return mPhysicalDevice;
	}
	[[nodiscard]] const vk::Device& GetDevice() const { return *mDevice; }

	[[nodiscard]] const vk::Queue& GetGraphicsQueue() const {
		return mGraphicsQueue;
	}
	[[nodiscard]] const vk::Queue& GetPresentQueue() const {
		return mPresentQueue;
	}

	[[nodiscard]] const QueueFamilyIndices& GetQueueFamilies() const {
		return mQueueFamilies;
	}

private:
	void PickPhysicalDevice(vk::Instance instance, vk::SurfaceKHR surface);
	void CreateLogicalDevice();

	vk::PhysicalDevice mPhysicalDevice;
	vk::UniqueDevice   mDevice;

	vk::Queue mGraphicsQueue;
	vk::Queue mPresentQueue;

	QueueFamilyIndices mQueueFamilies;
};

}  // namespace dubu::gfx