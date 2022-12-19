#version 430

layout(std140, binding = 0) uniform Transform {
    mat4 model;
    mat4 viewProj;
} mx;

in vec3 position;

void main()
{
    gl_Position = mx.viewProj * mx.model * vec4(position, 1.0);
}
