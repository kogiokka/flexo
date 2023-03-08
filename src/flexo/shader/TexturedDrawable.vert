#version 430

layout(std140, binding = 0) uniform Transform {
    mat4 model;
    mat4 viewProj;
} mx;

in vec3 position;
in vec3 normal;
in vec2 textureCoord;

out VertOut {
    vec3 position;
    vec3 normal;
    vec2 textureCoord;
} outData;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = mx.viewProj * mx.model * vec4(position, 1.0);

    outData.position = vec3(mx.model * vec4(position, 1.0));
    outData.normal = mat3(transpose(inverse(mx.model))) * normal;
    outData.textureCoord = textureCoord;
}
