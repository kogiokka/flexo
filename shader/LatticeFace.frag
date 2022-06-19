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
};

struct UniformBuffer {
    UniformBuffer_Vert vert;
    UniformBuffer_Frag frag;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 3) in vec2 texCoord;
layout (location = 4) uniform UniformBuffer ubo;

uniform sampler2D pattern;

in vec3 posFrag;
in vec3 normFrag;
in vec2 texCoordFrag;
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
  outColor = vec4((ambient + diffusion + specular), 1.0) * texture(pattern, texCoordFrag);
}
