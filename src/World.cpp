#include "World.hpp"
#include "common/Logger.hpp"

World world;

World::World()
{
    theModel = nullptr;
    // const char* imgFile = "res/images/checkerboard_64x64.png";
    const char* imgFile = "res/images/mandala.png";
    pattern = Image(imgFile, 0, STBI_rgb_alpha);
    Logger::info("Hello from World");

    if (pattern.data == nullptr) {
        Logger::error("Failed to open image: %s", imgFile);
        std::exit(EXIT_FAILURE);
    }

    lightPos = glm::vec3(0.0f, 5.0f, 0.0f);
    isWatermarked = false;
}

World::~World()
{
}
