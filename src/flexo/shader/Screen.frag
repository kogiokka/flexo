#version 430

out vec4 outColor;

in VertOut {
    vec2 textureCoord;
} inData;

layout (binding = 0) uniform sampler2D screenTexture;

void main()
{
    outColor = texture(screenTexture, inData.textureCoord);
}
