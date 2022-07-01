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

using RenderFlag = int;

enum RenderFlag_ : int {
    RenderFlag_DrawModel = 1 << 0,
    RenderFlag_DrawLatticeVertex = 1 << 1,
    RenderFlag_DrawLatticeEdge = 1 << 2,
    RenderFlag_DrawLatticeFace = 1 << 3,
    RenderFlag_DrawLightSource = 1 << 4,
};

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
    RenderFlag renderFlags;
};

extern World world;

#endif
