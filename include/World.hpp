#ifndef WORLD_H
#define WORLD_H

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Image.hpp"
#include "Lattice.hpp"
#include "Mesh.hpp"
#include "RandomIntNumber.hpp"
#include "VolumeData.hpp"

struct VolumetricModel {
    VolumeData data;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> textureCoords;
};

struct World {
    World();
    ~World();

    float modelColorAlpha = 0.8f;
    std::unique_ptr<InputData> dataset;

    std::unique_ptr<Lattice> lattice;

    Mesh uvsphere;
    Mesh cube;
    Mesh latticeMesh;
    std::vector<glm::vec3> neuronPositions;
    std::vector<unsigned int> latticeEdges;
    std::unique_ptr<Mesh> polyModel;
    std::unique_ptr<VolumetricModel> volModel;
    Image pattern;
};

extern World world;

#endif
