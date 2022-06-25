#version 430

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffusion;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    vec3 diffusion;
    vec3 specular;
    float shininess;
};

struct UniformBuffer_Vert {
    mat4 viewProjMat;
    mat4 modelMat;
};

struct UniformBuffer_Frag {
    Light light;
    Material material;
    vec3 viewPos;
    float alpha;
};

layout(std140, binding = 0) uniform UniformBuffer {
    UniformBuffer_Vert vert;
    UniformBuffer_Frag frag;
} ubo;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 translation;

out vec3 posFrag;
out vec3 normFrag;

void main()
{
    gl_Position = ubo.vert.viewProjMat * (vec4(translation, 0.0) + ubo.vert.modelMat * vec4(position, 1.0));
    posFrag = position;
    normFrag= normal;
}