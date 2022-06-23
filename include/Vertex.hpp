#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>

using Position = glm::vec3;
using Normal = glm::vec3;
using Color = glm::vec3;
using TextureCoord = glm::vec2;

struct VertexPN {
    Position position;
    Normal normal;
    static float* Offsets();
};

struct VertexPNC {
    Position position;
    Normal normal;
    Color color;
};

struct VertexPNT {
    Position position;
    Normal normal;
    TextureCoord texcoord;
};

#endif
