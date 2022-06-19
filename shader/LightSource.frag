#version 430

struct UniformBuffer_Vert {
    mat4 viewProjMat;
    mat4 modelMat;
};

struct UniformBuffer_Frag {
    vec3 lightColor;
};

struct UniformBuffer {
    UniformBuffer_Vert vert;
    UniformBuffer_Frag frag;
};

layout (location = 0) in vec3 position;
layout (location = 1) uniform UniformBuffer ubo;

out vec4 outColor;

void main()
{
    outColor = vec4(ubo.frag.lightColor, 1.0);
}
