#ifndef WORLD_H
#define WORLD_H

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

#include "Dataset.hpp"
#include "Map.hpp"
#include "Mesh.hpp"
#include "RandomIntNumber.hpp"

struct World {
    World();
    ~World();

    float modelColorAlpha = 0.8f;

    Mesh uvsphere;
    Mesh cube;
    Mesh mapMesh;
    Mesh neurons;

    std::shared_ptr<Mesh> theModel;
    std::shared_ptr<Dataset<3>> theDataset;
    std::shared_ptr<Map<3, 2>> theMap;
    std::vector<unsigned int> mapEdges;
    glm::vec3 lightPos;
    bool isWatermarked;
    std::string imagePath;
};

extern World world;

#endif
