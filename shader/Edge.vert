#version 450

layout (location = 0) in vec3 position;

uniform mat4 viewProjMat;
uniform mat4 modelMat;

void main()
{
  gl_Position = viewProjMat * modelMat * vec4(position, 1.0);
}
