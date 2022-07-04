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
    Light light;
    Material material;
    vec3 viewPos;
    bool isWatermarked;
};

layout(std140, binding = 0) uniform Transform {
    mat4 model;
    mat4 viewProj;
} mx;

layout(std140, binding = 1) uniform UniformBuffer {
    UniformBuffer_Vert vert;
} ubo;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoord;
layout (location = 3) in vec3 translation;

layout (binding = 0) uniform sampler2D voxelColor;
layout (binding = 1) uniform sampler2D voxelPattern;

out vec4 color;

void main()
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(ubo.vert.light.position - position);

    float diffuseCoef = max(dot(norm, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(ubo.vert.viewPos - position);
    float specularCoef = pow(max(dot(viewDir, reflectDir), 0.0), ubo.vert.material.shininess);

    vec3 ambient = ubo.vert.light.ambient * ubo.vert.material.ambient;
    vec3 diffusion = ubo.vert.light.diffusion * ubo.vert.material.diffusion * diffuseCoef;
    vec3 specular = ubo.vert.light.specular * ubo.vert.material.specular * specularCoef;

    if (ubo.vert.isWatermarked) {
        color = vec4((ambient + diffusion + specular), 1.0f) * texture(voxelPattern, textureCoord);
    } else {
        color = vec4((ambient + diffusion + specular), 1.0f) * texture(voxelColor, textureCoord);
    }

    gl_Position = mx.viewProj * (vec4(translation, 0.0) + mx.model * vec4(position, 1.0));
}

