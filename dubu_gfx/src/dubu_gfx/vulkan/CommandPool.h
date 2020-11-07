#pragma once

namespace dubu::gfx {

class CommandPool {
public:
	struct CreateInfo {
		vk::Device         device         = {};
		vk::PhysicalDevice physicalDevice = {};
		vk::SurfaceKHR     surface        = {};
	};

	vk::CommandPool GetCommandPool() const { return *mCommandPool; }

public:
	CommandPool(const CreateInfo& createInfo);

private:
	vk::UniqueCommandPool mCommandPool;
};

}  // namespace dubu::gfx