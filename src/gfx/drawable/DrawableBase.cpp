#include <utility>

#include "gfx/Renderer.hpp"
#include "gfx/Task.hpp"
#include "gfx/drawable/DrawableBase.hpp"

void DrawableBase::AddTask(Task task)
{
    m_tasks.push_back(std::move(task));
}

void DrawableBase::Submit(Renderer& renderer) const
{
    for (auto const& task : m_tasks) {
        renderer.Accept(task);
    }
};

void DrawableBase::SetVisible(bool visible)
{
    m_isVisible = visible;
}

bool DrawableBase::IsVisible() const
{
    return m_isVisible;
}

void DrawableBase::SetID(std::string id)
{
    m_id = id;
}

std::string DrawableBase::GetID() const
{
    return m_id;
}
