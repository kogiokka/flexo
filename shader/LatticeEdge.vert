#version 430

struct UniformBuffer_Vert {
    mat4 viewProjMat;
    mat4 modelMat;
};

struct UniformBuffer_Frag {
    vec3 color;
};

layout(std140, binding = 0) uniform UniformBuffer {
    UniformBuffer_Vert vert;
    UniformBuffer_Frag frag;
} ubo;

layout (location = 0) in vec3 position;

void main()
{
    gl_Position = ubo.vert.viewProjMat * ubo.vert.modelMat * vec4(position, 1.0);
}
