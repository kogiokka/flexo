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
