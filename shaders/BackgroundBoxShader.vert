#version 450
#extension GL_KHR_vulkan_glsl : enable
// #GL_ARB_separate_shader_onjects : enable

/*
*	TODO: Understand this shader better
*/

layout (set = 1, binding = 0) uniform UniformBufferObject {
	mat4 wvpMat; // world-view-projection matrix
	mat4 mMat;   // world matrix, converts from local space to global space
	mat4 nMat;   // normal transformation matrix
} ubo;

layout (location = 0) in vec3 inPosition; // vector containing the local coordinates
layout (location = 1) in vec3 inNormal;   // vector containing the coordinates of the normal 
layout (location = 2) in vec2 inUVText;	  // incoming vector of the UV coordinates

layout (location = 0) out vec2 fragTextureCoord;

void main (){
	
	fragTextureCoord = inUVText;
	gl_Position = ubo.wvpMat * vec4(inPosition, 1.0);
	
}
