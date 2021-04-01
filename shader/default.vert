#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 viewProjMat;
uniform mat4 modelMat;

out vec3 posFrag ;
out vec3 normFrag ;

void main()
{
  gl_Position = viewProjMat * modelMat * vec4(position, 1.0);
  posFrag = position;
  normFrag  = normal;
}
