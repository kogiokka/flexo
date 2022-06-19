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

out vec3 posFrag ;
out vec3 normFrag ;
out vec2 texCoordFrag ;

void main()
{
  gl_Position = ubo.vert.viewProjMat * ubo.vert.modelMat * vec4(position, 1.0);
  posFrag = vec3(ubo.vert.modelMat * vec4(position, 1.0));
  normFrag = mat3(transpose(inverse(ubo.vert.modelMat))) * normal;
  texCoordFrag = texCoord;
}
