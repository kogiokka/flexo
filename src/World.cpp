#include "World.hpp"

World world;

World::World()
{
    theMap = nullptr;

    lightPos = glm::vec3(0.0f, 5.0f, 0.0f);
}

World::~World()
{
}
