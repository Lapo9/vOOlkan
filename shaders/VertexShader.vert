#version 450 
#extension GL_KHR_vulkan_glsl : enable
// #GL_ARB_separate_shader_onjects : enable

/*
*	First shader implementation, this should be almost a final one
*/

layout (set = 1, binding = 0) uniform UniformBufferObject {
	mat4 wvpMat; // world-view-projection matrix
	mat4 mMat;   // world matrix, converts from local space to global space
	mat4 nMat;   // normal transformation matrix
} ubo;

layout (location = 0) in vec3 inPosition; // vector containing the local coordinates
layout (location = 1) in vec3 inNormal;   // vector containing the coordinates of the normal 
layout (location = 2) in vec2 inUVText;	  // incoming vector of the UV coordinates

layout (location = 0) out vec3 fragPos;   // vector containing the position of the point to pass to the fragment shader
layout (location = 1) out vec3 fragNormal; // vector containing the normal vector to pass to the fragment shader
layout (location = 2) out vec2 fragUVText; // vector for the UV coordinates of a texel

void main() {
	gl_Position = ubo.wvpMat * vec4(inPosition, 1.0); // vec4 is to transform local coordinates into homogeneous coordinates
	fragPos = (ubo.mMat * vec4(inPosition, 1.0)).xyz;  // after applying the transposition the w component is elicited to send screen coordinates
	fragNormal = mat3(ubo.nMat) * inNormal; // multiplication of the normal vector with the normal transformation matrix
	fragUVText = inUVText;	
}