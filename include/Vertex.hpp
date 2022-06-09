#pragma once

#include <glm/glm.hpp>

using Position = glm::vec3;
using Normal = glm::vec3;
using TextureCoord = glm::vec2;

struct VertexPN {
    Position position;
    Normal normal;
};

struct VertexPNT {
    Position position;
    Normal normal;
    TextureCoord texcoord;
};
