#ifndef WORLD_H
#define WORLD_H

#include <glm/glm.hpp>
#include <vector>

#include "Mesh.hpp"
#include "Vertex.hpp"

struct World {
    World();
    ~World();

    struct Lattice {
        std::vector<Vertex::Position> positions;
        std::vector<Vertex> faces;
        std::vector<unsigned int> indices;
    };

    float surfaceColorAlpha = 0.8f;
    Lattice lattice;

    Mesh uvsphere;
    Mesh surface;
};

extern World world;

#endif
