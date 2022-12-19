#version 430

struct UniformBuffer_Frag {
    vec3 color;
};

layout(std140, binding = 0) uniform Transform {
    mat4 model;
    mat4 viewProj;
} mx;

layout(std140, binding = 1) uniform UniformBuffer {
    UniformBuffer_Frag frag;
} ubo;

out vec4 outColor;

void main()
{
    outColor = vec4(ubo.frag.color, 1.0);
}
