#include "World.hpp"

World world;

World::World()
{
    theMap = nullptr;

    lightPos = glm::vec3(0.0f, 5.0f, 0.0f);
    imagePath = "res/images/mandala.png";
}

World::~World()
{
}
