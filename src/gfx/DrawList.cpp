#include <algorithm>
#include <array>
#include <cstdio>
#include <string>

#include "Project.hpp"
#include "gfx/DrawList.hpp"

#define X(type, name) name,
static std::string ObjectTypeNames[] = { OBJECT_TYPES };
#undef X

// Register factory: DrawList
static WatermarkingProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> SharedPtr<DrawList> { return std::make_shared<DrawList>(project); }
};

wxDEFINE_EVENT(EVT_ADD_OBJECT, wxCommandEvent);
wxDEFINE_EVENT(EVT_REMOVE_OBJECT, wxCommandEvent);

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
}

void DrawList::Add(enum ObjectType type, std::shared_ptr<DrawableBase> drawable)
{
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

    drawable->SetID(id);
    m_drawlist.push_back(drawable);

    wxCommandEvent event(EVT_ADD_OBJECT);
    event.SetString(id);
    m_project.ProcessEvent(event);
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
