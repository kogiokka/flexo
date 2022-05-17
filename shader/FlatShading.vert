#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 translation;

out vec4 color;

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

uniform Light light;
uniform Material material;
uniform vec3 viewPos;
uniform float alpha;

uniform mat4 viewProjMat;
uniform mat4 modelMat;

void main()
{
  vec3 norm = normalize(normal);
  vec3 lightDir = normalize(light.position - position);

  float diffuseCoef = max(dot(norm, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, norm);
  vec3 viewDir = normalize(viewPos - position);
  float specularCoef = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  vec3 ambient = light.ambient * material.ambient;
  vec3 diffusion = light.diffusion * material.diffusion * diffuseCoef;
  vec3 specular = light.specular * material.specular * specularCoef;

  color = vec4((ambient + diffusion + specular), alpha);
  gl_Position = viewProjMat * (vec4(translation, 0.0) + modelMat * vec4(position, 1.0));
}

