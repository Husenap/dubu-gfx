#pragma once

namespace dubu::gfx {

using blob = std::vector<char>;

class ShaderModule {
public:
	ShaderModule(vk::Device              device,
	             const blob&             byteCode,
	             vk::ShaderStageFlagBits shaderStage);

	vk::PipelineShaderStageCreateInfo GetPipelineShaderStageCreateInfo() const;

private:
	vk::ShaderStageFlagBits mShaderStage;
	vk::UniqueShaderModule  mShaderModule;
};

}  // namespace dubu::gfx