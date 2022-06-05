#include "World.hpp"
#include "common/Logger.hpp"

World world;

World::World()
{
    polyModel = nullptr;
    volModel = nullptr;
    const char* imgFile = "res/images/checkerboard_64x64.png";
    pattern = Image(imgFile, 0, STBI_rgb_alpha);
    Logger::info("Hello from World");

    if (pattern.data == nullptr) {
        Logger::error("Failed to open image: %s", imgFile);
        std::exit(EXIT_FAILURE);
    }
}

World::~World() { }
