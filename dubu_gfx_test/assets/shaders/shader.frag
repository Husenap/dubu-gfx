#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform sampler2D colorMap;
layout(set = 1, binding = 1) uniform sampler2D physicalDescriptorMap;
layout(set = 1, binding = 2) uniform sampler2D normalMap;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

const mat4 DitherPattern = mat4(0.0,
                                12.,
                                3.0,
                                15.,
                                8.0,
                                4.0,
                                11.,
                                7.0,
                                2.0,
                                14.,
                                1.0,
                                13.,
                                10.,
                                6.0,
                                9.0,
                                5.0);

void main() {
	vec4 colorSample = texture(colorMap, fragTexCoord).rgba;
	vec3 ARM         = texture(physicalDescriptorMap, fragTexCoord).rgb;

	if (colorSample.a < 0.5) {
		discard;
	}

	vec3 color = colorSample.rgb;

	float diffuse =
	    dot(fragNormal, normalize(vec3(1.0, 1.0, -1.0))) * 0.5 + 0.5;

	color *= diffuse;
	color *= ARM.r;

	float ditherValue =
	    DitherPattern[int(gl_FragCoord.y) % 4][int(gl_FragCoord.x) % 4] / 16.0;

	color += (1.0 / 255.0) * ditherValue;

	color    = pow(color, vec3(1.0 / 2.2));
	outColor = vec4(color, 1.0);
}
