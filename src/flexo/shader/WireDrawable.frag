#version 430

layout(std140, binding = 0) uniform Transform {
    mat4 model;
    mat4 viewProj;
} mx;

layout(std140, binding = 1) uniform UniformBuffer {
    vec3 color;
} ubo;

out vec4 outColor;

void main()
{
    outColor = vec4(ubo.color, 1.0);
}
