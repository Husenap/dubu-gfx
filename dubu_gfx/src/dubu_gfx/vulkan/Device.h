#pragma once

#include "Instance.h"

namespace dubu::gfx {

class Device {
public:
	Device(Instance& instance);

private:
	void PickPhysicalDevice(Instance& instance);
	void CreateLogicalDevice();

	vk::PhysicalDevice mPhysicalDevice;
	vk::UniqueDevice   mDevice;
	vk::Queue          mGraphicsQueue;
};

}  // namespace dubu::gfx