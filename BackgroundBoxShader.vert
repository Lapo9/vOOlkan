#version 450
// #GL_ARB_separate_shader_onjects : enable

/*
*	TODO: Understand this shader better
*/

layout (binding = 0) uniform UniformBufferObject {
	mat4 wvpMat; // world-view-projection matrix
	mat4 mMat;   // world matrix, converts from local space to global space
	mat4 nMat;   // normal transformation matrix
} ubo;

layout (location = 0) in vec3 inPosition; // vector containing the local coordinates
layout (location = 1) in vec3 inNormal;   // vector containing the coordinates of the normal 
layout (location = 2) in vec2 inUVText;	  // incoming vector of the UV coordinates

layout (location = 0) out vec3 fragTextureCoord;

void main (){
	
	fragTextureCoord = inPosition;
	vec4 position = ubo.wvpMat * vec4(inPosition, 1.0);
	gl_Position = position.xyww;
	
}
