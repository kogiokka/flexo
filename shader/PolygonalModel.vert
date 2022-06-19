#version 430

layout (std140, binding = 0) uniform UniformBuffer_Vert {
    mat4 viewProjMat;
    mat4 modelMat;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 posFrag;
out vec3 normFrag;

void main()
{
     gl_Position = viewProjMat * modelMat * vec4(position, 1.0);
     posFrag = position;
     normFrag  = normal;
}
