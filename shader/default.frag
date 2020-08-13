#version 450

in vec3 v_pos;
in vec3 v_norm;
out vec4 outColor;

uniform vec3 lightSrc;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float alpha;

void main()
{
  vec3 norm = normalize(v_norm);
  vec3 lightDir = normalize(lightSrc - v_pos);

  const float ambientCoef = 0.1;
  const float diffusionCoef = 0.8 * dot(norm, lightDir);
  const float specularCoef = 0.4;

  vec3 ambient = ambientCoef * lightColor;
  vec3 diffusion = diffusionCoef * lightColor;

  vec3 viewDir = normalize(viewPos - v_pos);
  vec3 reflectDir = reflect(-lightDir, norm);
  vec3 specular = specularCoef * pow(max(dot(viewDir, reflectDir), 0.0), 32) * lightColor;

  outColor = vec4((ambient + diffusion + specular), alpha);
}
