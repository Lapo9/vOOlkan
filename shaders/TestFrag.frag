#version 450

layout(set = 0, binding = 0) uniform Debug {
    vec3 test;
} debug;

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(debug.test.xy, 0.5, 1.0);
}