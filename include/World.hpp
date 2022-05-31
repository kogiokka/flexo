#ifndef WORLD_H
#define WORLD_H

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Image.hpp"
#include "Lattice.hpp"
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

    struct LatticeMesh {
        std::vector<Vertex::Position> positions;
        std::vector<Vertex2> faces;
        std::vector<unsigned int> indices;
    };

    float modelColorAlpha = 0.8f;
    std::unique_ptr<InputData> dataset;

    std::unique_ptr<Lattice> lattice;
    LatticeMesh latticeMesh;

    Mesh uvsphere;
    Mesh cube;
    std::unique_ptr<Mesh> polyModel;
    std::unique_ptr<VolumetricModel> volModel;
    Image pattern;
};

extern World world;

#endif
