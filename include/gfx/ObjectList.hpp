#ifndef DRAW_LIST_H
#define DRAW_LIST_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <wx/event.h>

#include "Attachable.hpp"
#include "Object.hpp"
#include "gfx/Renderer.hpp"

class WatermarkingProject;

#define OBJECT_TYPES                                                                                                   \
    X(ObjectType_Guides, "Guides")                                                                                     \
    X(ObjectType_Model, "Model")                                                                                       \
    X(ObjectType_Map, "Map")                                                                                           \
    X(ObjectType_Light, "Light")

#define X(type, name) type,
enum ObjectType : unsigned int { OBJECT_TYPES };
#undef X

wxDECLARE_EVENT(EVT_OBJECTLIST_DELETE_OBJECT, wxCommandEvent);

class ObjectList : public AttachableBase
{
public:
    static ObjectList& Get(WatermarkingProject& project);
    static ObjectList const& Get(WatermarkingProject const& project);
    ObjectList(WatermarkingProject& project);
    void Add(enum ObjectType type, std::shared_ptr<Object> object);
    void UpdateObjectDrawables(std::string const& id);
    void Submit(Renderer& renderer) const;

public:
    // Forward the iterator functions
    using iterator = std::vector<std::shared_ptr<Object>>::iterator;
    iterator begin();
    iterator end();
    using const_iterator = std::vector<std::shared_ptr<Object>>::const_iterator;
    const_iterator cbegin() const;
    const_iterator cend() const;

    using size_type = std::vector<std::shared_ptr<Object>>::size_type;
    size_type size() const;

private:
    void Remove(std::string id);
    void OnDeleteObject(wxCommandEvent& event);

    bool m_showHandle;
    std::vector<std::shared_ptr<Object>> m_list;
    std::unordered_map<enum ObjectType, unsigned int> m_typeCount;
    WatermarkingProject& m_project;
};

#endif
