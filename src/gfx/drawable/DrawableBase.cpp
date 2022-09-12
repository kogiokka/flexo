#include <utility>

#include "Task.hpp"
#include "gfx/Renderer.hpp"
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
