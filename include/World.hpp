#ifndef WORLD_H
#define WORLD_H

#include <memory>

#include "Dataset.hpp"
#include "object/Map.hpp"

struct World {
    World();
    ~World();

    std::shared_ptr<Dataset<3>> theDataset;
    std::shared_ptr<Map<3, 2>> theMap;
};

extern World world;

#endif
