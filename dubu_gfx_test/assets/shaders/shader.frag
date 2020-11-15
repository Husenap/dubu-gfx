#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
	vec3 color = fragColor;

	color *= texture(texSampler, fragTexCoord).rgb;

	color    = pow(color, vec3(1.0 / 2.2));
	outColor = vec4(color, 1.0);
}
