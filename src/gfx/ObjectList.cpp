#include <algorithm>
#include <array>
#include <cstdio>
#include <string>

#include "Project.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/ObjectList.hpp"
#include "pane/SceneOutlinerPane.hpp"

#define X(type, name) name,
static std::string ObjectTypeNames[] = { OBJECT_TYPES };
#undef X

// Register factory: ObjectList
static WatermarkingProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> SharedPtr<ObjectList> { return std::make_shared<ObjectList>(project); }
};

wxDEFINE_EVENT(EVT_OBJECTLIST_DELETE_OBJECT, wxCommandEvent);

ObjectList& ObjectList::Get(WatermarkingProject& project)
{
    return project.AttachedObjects::Get<ObjectList>(factoryKey);
}

ObjectList const& ObjectList::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

ObjectList::ObjectList(WatermarkingProject& project)
    : m_list()
    , m_project(project)
{
    m_showHandle = true;
    m_project.Bind(EVT_OBJECTLIST_DELETE_OBJECT, &ObjectList::OnDeleteObject, this);
}

void ObjectList::Add(enum ObjectType type, std::shared_ptr<Object> object)
{
    // FIXME
    std::string id = ObjectTypeNames[type];
    Remove(id);

    /* auto it = m_typeCount.find(type);

    std::string id;
    if (it == m_typeCount.end()) {
        m_typeCount.emplace(type, 0);
        id = ObjectTypeNames[type];
    } else {
        it->second += 1;
        char buf[4];
        snprintf(buf, 4, "%03u", it->second);
        id = ObjectTypeNames[type] + "." + std::string(buf);
    }*/

    object->SetID(id);
    object->GenerateDrawables(Graphics::Get(m_project));
    m_list.push_back(object);

    wxCommandEvent event(EVT_OUTLINER_ADD_OBJECT);
    event.SetString(id);
    m_project.ProcessEvent(event);
}

void ObjectList::Remove(std::string id)
{
    for (auto it = m_list.begin(); it != m_list.end();) {
        if ((*it)->GetID() == id) {
            m_list.erase(it);
            break;
        } else {
            it++;
        }
    }

    wxCommandEvent event(EVT_OUTLINER_DELETE_OBJECT);
    event.SetString(id);
    m_project.ProcessEvent(event);
}

void ObjectList::OnDeleteObject(wxCommandEvent& event)
{
    for (auto it = m_list.begin(); it != m_list.end();) {
        if ((*it)->GetID() == event.GetString()) {
            m_list.erase(it);
            break;
        } else {
            it++;
        }
    }

    Submit(Renderer::Get(m_project));
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
