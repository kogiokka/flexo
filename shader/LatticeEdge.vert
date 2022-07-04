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

layout (location = 0) in vec3 position;

void main()
{
    gl_Position = mx.viewProj * mx.model * vec4(position, 1.0);
}
