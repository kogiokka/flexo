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

struct VolModel {
    VolumeData data;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> textureCoords;
};

struct World {
    World();
    ~World();

    float modelColorAlpha = 0.8f;

    Mesh uvsphere;
    Mesh cube;
    Mesh latticeMesh;
    Mesh neurons;
    std::vector<unsigned int> latticeEdges;
    std::unique_ptr<Mesh> polyModel;
    std::unique_ptr<VolModel> volModel;
    Image pattern;
    glm::vec3 lightPos;
    bool isWatermarked;
};

extern World world;

#endif
