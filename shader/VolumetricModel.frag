#version 430

in VertOut {
    vec4 color;
} inData;

out vec4 outColor;

void main()
{
    outColor = inData.color;
}
