#version 430

in vec2 position;
in vec2 textureCoord;

out VertOut {
    vec2 textureCoord;
} outData;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    outData.textureCoord = textureCoord;
}
