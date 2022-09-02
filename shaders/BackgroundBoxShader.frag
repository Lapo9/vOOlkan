#version 450
// #GL_ARB_separate_shader_onjects : enable

/*
*	TODO: Understand this shader better
*/

layout (location = 0) in vec3 fragTextureCoord;

layout (location = 0) out vec4 outColor;

layout (binding = 1) uniform samplerCube background;

void main (){
	outColor = texture(background, fragTextureCoord);
}