#version 430

struct UniformBuffer_Vert {
    mat4 viewProjMat;
    mat4 modelMat;
};

struct UniformBuffer_Frag {
    vec3 lightColor;
};

layout (std140, binding = 0) uniform UniformBuffer {
    UniformBuffer_Vert vert;
    UniformBuffer_Frag frag;
} ubo;

layout (location = 0) in vec3 position;

out vec4 outColor;

void main()
{
    outColor = vec4(ubo.frag.lightColor, 1.0);
}
