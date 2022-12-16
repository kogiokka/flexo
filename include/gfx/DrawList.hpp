#ifndef DRAW_LIST_H
#define DRAW_LIST_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <wx/event.h>

#include "Attachable.hpp"
#include "gfx/Renderer.hpp"
#include "gfx/drawable/DrawableBase.hpp"

class WatermarkingProject;

#define OBJECT_TYPES                                                                                                   \
    X(ObjectType_Model, "Model")                                                                                       \
    X(ObjectType_MapFace, "Map.face")                                                                                  \
    X(ObjectType_MapEdge, "Map.edge")                                                                                  \
    X(ObjectType_MapVertex, "Map.vertex")                                                                              \
    X(ObjectType_Light, "Light")

#define X(type, name) type,
enum ObjectType : unsigned int { OBJECT_TYPES };
#undef X

wxDECLARE_EVENT(EVT_DRAWLIST_DELETE_OBJECT, wxCommandEvent);

class DrawList : public AttachableBase
{
public:
    static DrawList& Get(WatermarkingProject& project);
    static DrawList const& Get(WatermarkingProject const& project);
    DrawList(WatermarkingProject& project);
    void Add(enum ObjectType type, std::shared_ptr<DrawableBase> drawable);
    void Submit(Renderer& renderer) const;

public:
    // Forward the iterator functions
    using iterator = std::vector<std::shared_ptr<DrawableBase>>::iterator;
    iterator begin();
    iterator end();
    using const_iterator = std::vector<std::shared_ptr<DrawableBase>>::const_iterator;
    const_iterator cbegin() const;
    const_iterator cend() const;

private:
    void Remove(std::string id);
    void OnDeleteObject(wxCommandEvent& event);

    std::vector<std::shared_ptr<DrawableBase>> m_drawlist;
    std::unordered_map<enum ObjectType, unsigned int> m_typeCount;
    WatermarkingProject& m_project;
};

#endif
