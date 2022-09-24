#include "World.hpp"
#include "common/Logger.hpp"

World world;

World::World()
{
    theModel = nullptr;
    Logger::info("Hello from World");
    lightPos = glm::vec3(0.0f, 5.0f, 0.0f);
    isWatermarked = false;
    imagePath = "res/images/mandala.png";
}

World::~World()
{
}
