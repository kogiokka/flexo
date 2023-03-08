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

in vec3 position;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = mx.viewProj * mx.model * vec4(position, 1.0);
}

