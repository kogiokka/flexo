#ifndef WORLD_H
#define WORLD_H

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Mesh.hpp"
#include "RandomIntNumber.hpp"
#include "Vertex.hpp"
#include "VolumeData.hpp"

struct VolumetricModel {
    VolumeData data;
    std::vector<Vertex::Position> positions;
};

struct World {
    World();
    ~World();

    struct Lattice {
        std::vector<Vertex::Position> positions;
        std::vector<Vertex> faces;
        std::vector<unsigned int> indices;
    };

    float modelColorAlpha = 0.8f;
    Lattice lattice;

    Mesh uvsphere;
    Mesh cube;
    std::unique_ptr<Mesh> polyModel;
    std::unique_ptr<VolumetricModel> volModel;
};

extern World world;

#endif
