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
public:
    static MapList& Get(WatermarkingProject& project);
    static MapList const& Get(WatermarkingProject const& project);

    explicit MapList(WatermarkingProject& project);
    MapList(MapList const&) = delete;
    MapList& operator=(MapList const&) = delete;

private:
    WatermarkingProject& m_project;
};

extern WatermarkingProject::AttachedObjects::RegisteredFactory const map32ListKey;

template <>
MapList<3, 2>& MapList<3, 2>::Get(WatermarkingProject& project);

template <>
MapList<3, 2> const& MapList<3, 2>::Get(WatermarkingProject const& project);

template <int InDim, int OutDim>
MapList<InDim, OutDim>::MapList(WatermarkingProject& project)
    : m_project(project)
{
}

#endif
