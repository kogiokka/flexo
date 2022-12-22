#include "Map32List.hpp"

// Register factory: Map
WatermarkingProject::AttachedObjects::RegisteredFactory const map32ListKey {
    [](WatermarkingProject& project) -> SharedPtr<MapList<3, 2>> { return std::make_shared<MapList<3, 2>>(project); }
};

template <>
MapList<3, 2>& MapList<3, 2>::Get(WatermarkingProject& project)
{
    return project.AttachedObjects::Get<MapList>(map32ListKey);
}

template <>
MapList<3, 2> const& MapList<3, 2>::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

template <>
void MapList<3, 2>::Add(Vec2i size, MapFlags flags, MapInitState initState, BoundingBox box)
{
    auto map = std::make_shared<Map<3, 2>>();

    int width = size.x;
    int height = size.y;

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
                map->nodes.emplace_back(Vec3f { box.min.x + i * dx, box.min.y + j * dy, box.max.z },
                                        Vec2f { static_cast<float>(i), static_cast<float>(j) }, Vec2f { i / w, j / h });
            }
        }
    }

    map->size.x = width;
    map->size.y = height;
    map->flags = flags;

    emplace_back(std::move(map));
}
