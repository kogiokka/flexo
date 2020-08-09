#version 450

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

uniform mat4 view_proj_matrix;

out vec3 v_color;

void main()
{
  gl_Position = view_proj_matrix * vec4(position, 0.0, 1.0);
  v_color = color;
}
