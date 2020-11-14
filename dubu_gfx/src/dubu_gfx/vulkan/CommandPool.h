#pragma once

#include "QueueFamilyIndices.h"

namespace dubu::gfx {

class CommandPool {
public:
	struct CreateInfo {
		vk::Device         device        = {};
		QueueFamilyIndices queueFamilies = {};
	};

	[[nodiscard]] const vk::CommandPool& GetCommandPool() const {
		return *mCommandPool;
	}

public:
	CommandPool(const CreateInfo& createInfo);

private:
	vk::UniqueCommandPool mCommandPool;
};

}  // namespace dubu::gfx