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
    float alpha;
};

layout(std140, binding = 0) uniform Transform {
    mat4 model;
    mat4 viewProj;
} mx;

layout (std140, binding = 1) uniform UniformBuffer {
    UniformBuffer_Frag frag;
} ubo;

in vec3 posFrag;
in vec3 normFrag;
out vec4 outColor;

void main()
{
    vec3 norm = normalize(normFrag);
    vec3 lightDir = normalize(ubo.frag.light.position - posFrag);

    float diffuseCoef = dot(norm, lightDir);
    if (diffuseCoef < 0.0) {
        diffuseCoef *= -1.0;
        norm *= -1.0;
    }

    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(ubo.frag.viewPos - posFrag);
    float specularCoef = pow(max(dot(viewDir, reflectDir), 0.0), ubo.frag.material.shininess);

    vec3 ambient = ubo.frag.light.ambient * ubo.frag.material.ambient;
    vec3 diffusion = ubo.frag.light.diffusion * ubo.frag.material.diffusion * diffuseCoef;
    vec3 specular = ubo.frag.light.specular * ubo.frag.material.specular * specularCoef;
    outColor = vec4((ambient + diffusion + specular), ubo.frag.alpha);
}
