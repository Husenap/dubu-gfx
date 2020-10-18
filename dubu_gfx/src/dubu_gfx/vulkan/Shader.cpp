#include "Shader.h"

namespace dubu::gfx {

Shader::Shader(const CreateInfo& ci) {
	for (std::size_t i = 0; i < ci.shaderModules.size(); ++i) {
		auto& [byteCode, type] = ci.shaderModules[i];

		mShaderModules.push_back(
		    std::make_unique<ShaderModule>(ci.device, byteCode, type));

		mShaderStages.push_back(
		    mShaderModules[i]->GetPipelineShaderStageCreateInfo());
	}
}

}  // namespace dubu::gfx