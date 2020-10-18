#pragma once

#include "Instance.h"

namespace dubu::gfx {

class Device {
public:
	Device(vk::Instance instance, vk::SurfaceKHR surface);

	vk::PhysicalDevice GetPhysicalDevice() const { return mPhysicalDevice; }
	vk::Device         GetDevice() const { return *mDevice; }

private:
	void PickPhysicalDevice(vk::Instance instance, vk::SurfaceKHR surface);
	void CreateLogicalDevice(vk::SurfaceKHR surface);

	vk::PhysicalDevice mPhysicalDevice;
	vk::UniqueDevice   mDevice;

	vk::Queue mGraphicsQueue;
	vk::Queue mPresentQueue;

};

}  // namespace dubu::gfx