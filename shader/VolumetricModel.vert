#version 430


layout(std140, binding = 0) uniform Transform {
    mat4 model;
    mat4 viewProj;
} mx;

layout(std140, binding = 1) uniform Light {
    vec3 position;
    vec3 ambient;
    vec3 diffusion;
    vec3 specular;
} light;

layout(std140, binding = 2) uniform Material {
    vec3 ambient;
    vec3 diffusion;
    vec3 specular;
    float shininess;
} material;

layout(std140, binding = 3) uniform UniformBuffer {
    vec3 viewPos;
    bool isWatermarked;
} ubo;

in vec3 position;
in vec3 normal;
in vec2 textureCoord;

layout (binding = 0) uniform sampler2D voxelPattern;
layout (binding = 1) uniform sampler2D voxelColor;

out VertOut {
    vec4 color;
} outData;

void main()
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - position);

    float diffuseCoef = max(dot(norm, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(ubo.viewPos - position);
    float specularCoef = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * material.ambient;
    vec3 diffusion = light.diffusion * material.diffusion * diffuseCoef;
    vec3 specular = light.specular * material.specular * specularCoef;

    if (ubo.isWatermarked) {
        outData.color = vec4((ambient + diffusion + specular), 1.0f) * texture(voxelPattern, textureCoord);
    } else {
        outData.color = vec4((ambient + diffusion + specular), 1.0f) * texture(voxelColor, textureCoord);
    }

    gl_Position = mx.viewProj * mx.model * vec4(position, 1.0);
}

