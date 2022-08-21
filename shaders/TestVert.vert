#version 450

layout(set = 1, binding = 0) uniform MVP {
    mat4 mvp;
} mvp;

layout(set = 1, binding = 1) uniform Color {
    vec4 rgb;
} color;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = mvp.mvp * vec4(inPosition, 1.0f);
    fragColor = color.rgb;
}