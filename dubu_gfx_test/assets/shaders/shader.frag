#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform sampler2D colorMap;
layout(set = 1, binding = 1) uniform sampler2D physicalDescriptorMap;
layout(set = 1, binding = 2) uniform sampler2D normalMap;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
	vec3 color = texture(colorMap, fragTexCoord).rgb;

	color    = pow(color, vec3(1.0 / 2.2));
	outColor = vec4(color, 1.0);
}
