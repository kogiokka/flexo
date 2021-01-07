#version 430

in vec3 v_pos;
in vec3 v_norm;
out vec4 outColor;

struct Light {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform Light light;
uniform vec3 viewPos;
uniform float alpha;
uniform vec3 lightColor;

void
main()
{
  vec3 norm = normalize(v_norm);
  vec3 lightDir = normalize(light.direction);

  float diffCoef = dot(norm, -lightDir);
  if (diffCoef < 0.0) {
    diffCoef *= -1.0;
    norm *= -1.0;
  }
  vec3 diffusion = light.diffuse * diffCoef * lightColor;
  vec3 ambient = light.ambient * lightColor;

  vec3 reflectDir = reflect(lightDir, norm);
  vec3 viewDir = normalize(viewPos - v_pos);
  float specCoef = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = light.specular * specCoef * lightColor;

  outColor = vec4((ambient + diffusion + specular), alpha);
}
