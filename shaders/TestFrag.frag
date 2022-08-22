#version 450

layout(set = 0, binding = 0) uniform sampler2D texture1;


layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec4 outColor;

void main() {
    //outColor = vec4(uv, 0.0, 1.0);
    outColor = vec4(fragColor * texture(texture1, uv).rgb, 1.0f);
}