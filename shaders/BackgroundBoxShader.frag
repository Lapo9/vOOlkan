#version 450
#extension GL_KHR_vulkan_glsl : enable
// #GL_ARB_separate_shader_onjects : enable

/*
*	TODO: Understand this shader better
*/

layout (location = 0) in vec2 fragTextureCoord;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform sampler2D background;

void main (){
	outColor = vec4(texture(background, fragTextureCoord).rgb, 1.0f);
}