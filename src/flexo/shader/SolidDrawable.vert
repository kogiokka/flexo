#version 430

layout(std140, binding = 0) uniform Transform {
    mat4 model;
    mat4 viewProj;
} mx;

in vec3 position;
in vec3 normal;

out VertOut {
    vec3 position;
    vec3 normal;
} outData;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
     gl_Position = mx.viewProj * mx.model * vec4(position, 1.0);
     outData.position = position;
     outData.normal = normal;
}
