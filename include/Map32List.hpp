#ifndef MAP32_LIST_H
#define MAP32_LIST_H

#include <memory>
#include <utility>
#include <vector>

#include "Attachable.hpp"
#include "Dataset.hpp"
#include "Project.hpp"
#include "RandomRealNumber.hpp"
#include "object/Map.hpp"

class WatermarkingProject;

template <int InDim, int OutDim>
using ListOfMap = std::vector<std::shared_ptr<Map<InDim, OutDim>>>;

template <int InDim, int OutDim>
class MapList final : public AttachableBase, public ListOfMap<InDim, OutDim>
{
    // privatize emplace_back and push_back method, use Add instead.
    using ListOfMap<InDim, OutDim>::emplace_back;
    using ListOfMap<InDim, OutDim>::push_back;

public:
    static MapList& Get(WatermarkingProject& project);
    static MapList const& Get(WatermarkingProject const& project);

    explicit MapList(WatermarkingProject& project);
    MapList(MapList const&) = delete;
    MapList& operator=(MapList const&) = delete;
    void Add(Vec<OutDim, int> size, MapFlags flags, MapInitState initState = MapInitState_Random,
             BoundingBox box = { { -100.0f, -100.0f, -100.0f }, { 100.0f, 100.0f, 100.0f } });

private:
    WatermarkingProject& m_project;
};

extern WatermarkingProject::AttachedObjects::RegisteredFactory const map32ListKey;

template <>
MapList<3, 2>& MapList<3, 2>::Get(WatermarkingProject& project);

template <>
MapList<3, 2> const& MapList<3, 2>::Get(WatermarkingProject const& project);

template <>
void MapList<3, 2>::Add(Vec2i size, MapFlags flags, MapInitState initState, BoundingBox box);

template <int InDim, int OutDim>
MapList<InDim, OutDim>::MapList(WatermarkingProject& project)
    : m_project(project)
{
}

#endif
