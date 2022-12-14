#include <array>
#include <cassert>
#include <memory>
#include <utility>

#include "Map.hpp"
#include "Project.hpp"
#include "RandomRealNumber.hpp"

// Register factory: Map
static WatermarkingProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> SharedPtr<MapList> { return std::make_shared<MapList>(project); }
};

MapList& MapList::Get(WatermarkingProject& project)
{
    return project.AttachedObjects::Get<MapList>(factoryKey);
}

MapList const& MapList::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

MapList::MapList(WatermarkingProject& project)
    : m_project(project)
{
}

void MapList::Add(int width, int height, MapFlags flags, MapInitState initState, BoundingBox box)
{
    using std::array;
    auto map = std::make_shared<Map<3, 2>>();

    float const w = static_cast<float>(width - 1);
    float const h = static_cast<float>(height - 1);

    if (initState == MapInitState_Random) {
        RandomRealNumber<float> xRng(box.min.x, box.max.x);
        RandomRealNumber<float> yRng(box.min.y, box.max.y);
        RandomRealNumber<float> zRng(box.min.z, box.max.z);

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                map->nodes.emplace_back(Vec3f { xRng.scalar(), yRng.scalar(), zRng.scalar() },
                                        Vec2f { static_cast<float>(i), static_cast<float>(j) }, Vec2f { i / w, j / h });
            }
        }
    } else if (initState == MapInitState_Plane) {
        float dx = (box.max.x - box.min.x) / static_cast<float>(width);
        float dy = (box.max.y - box.min.y) / static_cast<float>(height);

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                map->nodes.emplace_back(Vec3f { i * dx, j * dy, box.max.z },
                                        Vec2f { static_cast<float>(i), static_cast<float>(j) }, Vec2f { i / w, j / h });
            }
        }
    }

    map->width = width;
    map->height = height;
    map->flags = flags;

    emplace_back(std::move(map));
}
