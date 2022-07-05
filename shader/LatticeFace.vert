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

struct UniformBuffer_Frag {
    Light light;
    Material material;
    vec3 viewPos;
};

layout(std140, binding = 0) uniform Transform {
    mat4 model;
    mat4 viewProj;
} mx;

layout(std140, binding = 1) uniform UniformBuffer {
    UniformBuffer_Frag frag;
} ubo;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoord;

out VertOut {
    vec3 position;
    vec3 normal;
    vec2 textureCoord;
} outData;

void main()
{
    gl_Position = mx.viewProj * mx.model * vec4(position, 1.0);

    outData.position = vec3(mx.model * vec4(position, 1.0));
    outData.normal = mat3(transpose(inverse(mx.model))) * normal;
    outData.textureCoord = textureCoord;
}
