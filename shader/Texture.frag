#version 430

in vec3 posFrag;
in vec3 normFrag;
in vec2 texCoordFrag;
out vec4 outColor;

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
uniform sampler2D pattern;

void main()
{
  vec3 norm = normalize(normFrag);
  vec3 lightDir = normalize(light.position - posFrag);

  float diffuseCoef = dot(norm, lightDir);
  if (diffuseCoef < 0.0) {
    diffuseCoef *= -1.0;
    norm *= -1.0;
  }

  vec3 reflectDir = reflect(-lightDir, norm);
  vec3 viewDir = normalize(viewPos - posFrag);
  float specularCoef = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  vec3 ambient = light.ambient * material.ambient;
  vec3 diffusion = light.diffusion * material.diffusion * diffuseCoef;
  vec3 specular = light.specular * material.specular * specularCoef;
  outColor = vec4((ambient + diffusion + specular), 1.0) * texture(pattern, texCoordFrag);
}
