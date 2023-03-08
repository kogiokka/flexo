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
} ubo;

layout (binding = 0) uniform sampler2D pattern;

in VertOut {
    vec3 position;
    vec3 normal;
    vec2 textureCoord;
} inData;

out vec4 outColor;

void main()
{
  vec3 norm = normalize(inData.normal);
  vec3 lightDir = normalize(light.position - inData.position);

  float diffuseCoef = dot(norm, lightDir);
  if (diffuseCoef < 0.0) {
    diffuseCoef *= -1.0;
    norm *= -1.0;
  }

  vec3 reflectDir = reflect(-lightDir, norm);
  vec3 viewDir = normalize(ubo.viewPos - inData.position);
  float specularCoef = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  vec3 ambient = light.ambient * material.ambient;
  vec3 diffusion = light.diffusion * material.diffusion * diffuseCoef;
  vec3 specular = light.specular * material.specular * specularCoef;
  outColor = vec4((ambient + diffusion + specular), 1.0f) * texture(pattern, inData.textureCoord);
}
