#pragma once

#include "ShaderModule.h"

namespace dubu::gfx {

class Shader {
public:
	struct CreateInfo {
		vk::Device                                             device;
		std::vector<std::tuple<blob, vk::ShaderStageFlagBits>> shaderModules;
	};

public:
	Shader(const CreateInfo& ci);

	[[nodiscard]] const auto& GetShaderStages() const { return mShaderStages; }

private:
	std::vector<vk::PipelineShaderStageCreateInfo> mShaderStages;
	std::vector<std::unique_ptr<ShaderModule>>     mShaderModules;
};

}  // namespace dubu::gfx