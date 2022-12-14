#ifndef MAP_H
#define MAP_H

#include <memory>
#include <vector>

#include <wx/event.h>

#include "Attachable.hpp"
#include "Dataset.hpp"
#include "Node.hpp"

using MapFlags = int;

enum MapFlags_ : int {
    MapFlags_CyclicNone = 1 << 0,
    MapFlags_CyclicX = 1 << 1,
    MapFlags_CyclicY = 1 << 2,
};

typedef enum {
    MapInitState_Plane,
    MapInitState_Random,
} MapInitState;

template <int InDim, int OutDim>
struct Map {
    Vec<OutDim, int> size;
    std::vector<Node<InDim, OutDim>> nodes;
    MapFlags flags;
};

class WatermarkingProject;
using ListOfMap = std::vector<std::shared_ptr<Map<3, 2>>>;

class MapList final : public AttachableBase, public ListOfMap
{
    // privatize emplace_back and push_back method, use Add instead.
    using ListOfMap::emplace_back;
    using ListOfMap::push_back;

public:
    static MapList& Get(WatermarkingProject& project);
    static MapList const& Get(WatermarkingProject const& project);

    explicit MapList(WatermarkingProject& project);
    MapList(MapList const&) = delete;
    MapList& operator=(MapList const&) = delete;
    void Add(int width, int height, MapFlags flags, MapInitState initState = MapInitState_Random,
             BoundingBox box = { { -100.0f, -100.0f, -100.0f }, { 100.0f, 100.0f, 100.0f } });

private:
    WatermarkingProject& m_project;
};

#endif
