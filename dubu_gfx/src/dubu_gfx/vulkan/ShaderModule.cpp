#include "ShaderModule.h"

namespace dubu::gfx {

ShaderModule::ShaderModule(vk::Device              device,
                           const blob&             byteCode,
                           vk::ShaderStageFlagBits shaderStage) {
	mShaderStage  = shaderStage;
	mShaderModule = device.createShaderModuleUnique(vk::ShaderModuleCreateInfo{
	    .codeSize = byteCode.size(),
	    .pCode    = reinterpret_cast<const uint32_t*>(byteCode.data()),
	});
}

vk::PipelineShaderStageCreateInfo
ShaderModule::GetPipelineShaderStageCreateInfo() const {
	return vk::PipelineShaderStageCreateInfo{
	    .stage  = mShaderStage,
	    .module = *mShaderModule,
	    .pName  = "main",
	};
}

}  // namespace dubu::gfx