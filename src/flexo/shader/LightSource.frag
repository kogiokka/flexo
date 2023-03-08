#version 430

struct UniformBuffer_Frag {
    vec3 lightColor;
};

layout(std140, binding = 0) uniform Transform {
    mat4 model;
    mat4 viewProj;
} mx;

layout (std140, binding = 1) uniform UniformBuffer {
    UniformBuffer_Frag frag;
} ubo;

layout (location = 0) in vec3 position;

out vec4 outColor;

void main()
{
    outColor = vec4(ubo.frag.lightColor, 1.0);
}
