#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(set = 0, binding = 0) uniform sampler2D texture1;


layout(location = 0) in vec3 normal;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 color;

layout(location = 0) out vec4 outColor;

void main() {
    //outColor = vec4(uv, 0.0, 1.0);
    outColor = vec4(texture(texture1, uv).xyz * color, 1.0f);
}