#include <algorithm>

#include "Project.hpp"
#include "gfx/DrawList.hpp"

// Register factory: DrawList
static WatermarkingProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> SharedPtr<DrawList> { return std::make_shared<DrawList>(project); }
};

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

void DrawList::Add(std::shared_ptr<DrawableBase> drawable)
{
    m_drawlist.push_back(drawable);
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
