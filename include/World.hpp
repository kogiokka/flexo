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

struct World {
    World();
    ~World();

    float modelColorAlpha = 0.8f;

    Mesh uvsphere;
    Mesh cube;
    Mesh latticeMesh;
    Mesh neurons;
    std::shared_ptr<Mesh> theModel;
    std::vector<unsigned int> latticeEdges;
    Image pattern;
    glm::vec3 lightPos;
    bool isWatermarked;
};

extern World world;

#endif
