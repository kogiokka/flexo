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
    Light light;
    Material material;
    vec3 viewPos;
    float alpha;
};

layout(std140, binding = 0) uniform UniformBuffer {
    UniformBuffer_Vert vert;
} ubo;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 translation;
layout (location = 3) in vec2 textureCoord;

uniform sampler2D pattern;

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

    color = vec4((ambient + diffusion + specular), ubo.vert.alpha) * texture(pattern, textureCoord);

    gl_Position = ubo.vert.viewProjMat * (vec4(translation, 0.0) + ubo.vert.modelMat * vec4(position, 1.0));
}

