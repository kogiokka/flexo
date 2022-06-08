#pragma once

#include <glm/glm.hpp>
#include <type_traits>

struct Vertex {
    using Position = glm::vec3;
    using Normal = glm::vec3;

    Position position;
    Normal normal;
};

struct Vertex2 {
    using Position = glm::vec3;
    using Normal = glm::vec3;
    using TextureCoord = glm::vec2;

    Position position;
    Normal normal;
    TextureCoord texcoord;
};

static_assert(sizeof(Vertex) == 24);
