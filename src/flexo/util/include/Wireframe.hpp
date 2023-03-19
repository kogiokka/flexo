#ifndef WIREFRAME_H
#define WIREFRAME_H

#include <vector>

#include <glm/glm.hpp>

#include "Vec.hpp"

using Edge = Vec2<unsigned int>;

struct Wireframe {
    std::vector<glm::vec3> positions;
    std::vector<Edge> edges;
};

#endif
