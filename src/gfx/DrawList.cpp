#include <algorithm>
#include <array>
#include <cstdio>
#include <string>

#include "Project.hpp"
#include "gfx/DrawList.hpp"
#include "pane/SceneOutlinerPane.hpp"

#define X(type, name) name,
static std::string ObjectTypeNames[] = { OBJECT_TYPES };
#undef X

// Register factory: DrawList
static WatermarkingProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> SharedPtr<DrawList> { return std::make_shared<DrawList>(project); }
};

wxDEFINE_EVENT(EVT_DRAWLIST_DELETE_OBJECT, wxCommandEvent);

DrawList& DrawList::Get(WatermarkingProject& project)
{
    return project.AttachedObjects::Get<DrawList>(factoryKey);
}

DrawList const& DrawList::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

DrawList::DrawList(WatermarkingProject& project)
    : m_drawlist()
    , m_project(project)
{
    m_project.Bind(EVT_DRAWLIST_DELETE_OBJECT, &DrawList::OnDeleteObject, this);
}

void DrawList::Add(enum ObjectType type, std::shared_ptr<DrawableBase> drawable)
{
    // FIXME: Let the user handle all the deletion from the scene outliner
    std::string id = ObjectTypeNames[type];
    Remove(id);

    /*
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
    */

    drawable->SetID(id);
    m_drawlist.push_back(drawable);

    wxCommandEvent event(EVT_OUTLINER_ADD_OBJECT);
    event.SetString(id);
    m_project.ProcessEvent(event);
}

void DrawList::Remove(std::string id)
{
    for (auto it = m_drawlist.begin(); it != m_drawlist.end();) {
        if ((*it)->GetID() == id) {
            m_drawlist.erase(it);
            break;
        } else {
            it++;
        }
    }

    wxCommandEvent event(EVT_OUTLINER_DELETE_OBJECT);
    event.SetString(id);
    m_project.ProcessEvent(event);
}

void DrawList::OnDeleteObject(wxCommandEvent& event)
{
    Remove(event.GetString().ToStdString());
    Submit(Renderer::Get(m_project));
}

void DrawList::Submit(Renderer& renderer) const
{
    renderer.Clear();
    for (auto const& drawable : m_drawlist) {
        drawable->Submit(renderer);
    }
}

DrawList::iterator DrawList::begin()
{
    return m_drawlist.begin();
}

DrawList::iterator DrawList::end()
{
    return m_drawlist.end();
}

DrawList::const_iterator DrawList::cbegin() const
{
    return m_drawlist.cbegin();
}

DrawList::const_iterator DrawList::cend() const
{
    return m_drawlist.cend();
}
