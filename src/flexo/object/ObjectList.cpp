#include <algorithm>
#include <array>
#include <cstdio>
#include <string>

#include "Project.hpp"
#include "object/ObjectList.hpp"
#include "pane/SceneViewportPane.hpp"
#include "pane/SceneOutlinerPane.hpp"
#include "pane/SelfOrganizingMapPane.hpp"

#define X(type, name) name,
static std::string ObjectTypeNames[] = { OBJECT_TYPES };
#undef X

// Register factory: ObjectList
static FlexoProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](FlexoProject& project) -> SharedPtr<ObjectList> { return std::make_shared<ObjectList>(project); }
};

wxDEFINE_EVENT(EVT_OBJECTLIST_DELETE_OBJECT, wxCommandEvent);

ObjectList& ObjectList::Get(FlexoProject& project)
{
    return project.AttachedObjects::Get<ObjectList>(factoryKey);
}

ObjectList const& ObjectList::Get(FlexoProject const& project)
{
    return Get(const_cast<FlexoProject&>(project));
}

ObjectList::ObjectList(FlexoProject& project)
    : m_list()
    , m_project(project)
{
    m_project.Bind(EVT_OBJECTLIST_DELETE_OBJECT, &ObjectList::OnDeleteObject, this);
}

void ObjectList::Add(std::shared_ptr<Object> object)
{
    auto type = object->GetType();
    auto it = m_typeCount.find(type);

    std::string id;
    if (it == m_typeCount.end()) {
        m_typeCount.emplace(type, 0);
        id = ObjectTypeNames[type];
    } else {
        it->second += 1;
        char buf[4];
        snprintf(buf, 4, "%03u", it->second);
        id = ObjectTypeNames[type] + "." + std::string(buf);
    }

    object->SetID(id);
    object->GenerateDrawables(SceneViewportPane::Get(m_project).GetGL());
    m_list.push_back(object);

    wxCommandEvent event(EVT_OUTLINER_ADD_OBJECT);
    event.SetString(id);
    m_project.ProcessEvent(event);
}

void ObjectList::OnDeleteObject(wxCommandEvent& event)
{
    for (auto it = m_list.begin(); it != m_list.end();) {
        auto id = wxString((*it)->GetID());
        if (id == event.GetString()) {
            m_list.erase(it);
            break;
        } else {
            it++;
        }
    }
}

void ObjectList::Submit(Renderer& renderer) const
{
    renderer.Clear();
    for (auto const& obj : m_list) {
        for (auto const& drawable : obj->GetDrawList()) {
            drawable->Submit(renderer);
        }
    }
}

ObjectList::iterator ObjectList::begin()
{
    return m_list.begin();
}

ObjectList::iterator ObjectList::end()
{
    return m_list.end();
}

ObjectList::const_iterator ObjectList::cbegin() const
{
    return m_list.cbegin();
}

ObjectList::const_iterator ObjectList::cend() const
{
    return m_list.cend();
}

ObjectList::size_type ObjectList::size() const
{
    return m_list.size();
}
