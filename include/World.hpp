#ifndef WORLD_H
#define WORLD_H

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

#include "Dataset.hpp"
#include "Map.hpp"
#include "EditableMesh.hpp"
#include "RandomIntNumber.hpp"

struct World {
    World();
    ~World();

    EditableMesh mapMesh;

    std::shared_ptr<Dataset<3>> theDataset;
    std::shared_ptr<Map<3, 2>> theMap;
    glm::vec3 lightPos;
};

extern World world;

#endif
