#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 viewProjMat;
uniform mat4 modelMat;

out vec3 v_pos;
out vec3 v_norm;

void main()
{
  gl_Position = viewProjMat * modelMat * vec4(position, 1.0);
  v_pos = position;
  v_norm = normal;
}
