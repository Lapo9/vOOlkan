#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(set = 1, binding = 0) uniform Matrices{
    mat4 mvp;
    mat4 mMat;
    mat4 nMat;
} matrices;

layout(set = 1, binding = 1) uniform Color{
    vec3 rgb;
} color;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUvText;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragUvText;
layout(location = 2) out vec3 fragColor;


void main() {
    gl_Position = matrices.mvp * vec4(inPosition, 1.0f);
    fragNormal = mat3(matrices.nMat) * inNormal;
    fragUvText = inUvText;
    fragColor = color.rgb;
}