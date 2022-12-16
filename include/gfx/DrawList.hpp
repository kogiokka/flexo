#ifndef DRAW_LIST_H
#define DRAW_LIST_H

#include <memory>
#include <unordered_map>
#include <vector>

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

wxDECLARE_EVENT(EVT_ADD_OBJECT, wxCommandEvent);
wxDECLARE_EVENT(EVT_REMOVE_OBJECT, wxCommandEvent);

class DrawList : public AttachableBase
{
public:
    static DrawList& Get(WatermarkingProject& project);
    static DrawList const& Get(WatermarkingProject const& project);
    DrawList(WatermarkingProject& project);
    void Add(enum ObjectType type, std::shared_ptr<DrawableBase> drawable);
    template <typename T>
    void Remove();
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
    std::vector<std::shared_ptr<DrawableBase>> m_drawlist;
    std::unordered_map<enum ObjectType, unsigned int> m_typeCount;
    WatermarkingProject& m_project;
};

template <typename T>
void DrawList::Remove()
{
    bool (*const FindDrawable)(std::shared_ptr<DrawableBase>&) = [](std::shared_ptr<DrawableBase>& drawable) {
        auto* ptr = drawable.get();
        return (dynamic_cast<T*>(ptr) != nullptr);
    };

    m_drawlist.erase(std::remove_if(m_drawlist.begin(), m_drawlist.end(), FindDrawable), m_drawlist.end());

    wxCommandEvent event(EVT_REMOVE_OBJECT);
    m_project.ProcessEvent(event);
}

#endif
