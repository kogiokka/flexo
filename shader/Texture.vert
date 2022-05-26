#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 3) in vec2 texCoord;

uniform mat4 viewProjMat;
uniform mat4 modelMat;

out vec3 posFrag ;
out vec3 normFrag ;
out vec2 texCoordFrag ;

void main()
{
  gl_Position = viewProjMat * modelMat * vec4(position, 1.0);
  posFrag = vec3(modelMat * vec4(position, 1.0));
  normFrag = mat3(transpose(inverse(modelMat))) * normal;
  texCoordFrag = texCoord;
}
