#ifndef WORLD_H
#define WORLD_H

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

#include "Dataset.hpp"
#include "Map.hpp"

struct World {
    World();
    ~World();

    std::shared_ptr<Dataset<3>> theDataset;
    std::shared_ptr<Map<3, 2>> theMap;
};

extern World world;

#endif
