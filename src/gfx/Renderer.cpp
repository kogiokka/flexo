#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Project.hpp"
#include "World.hpp"
#include "common/Logger.hpp"
#include "gfx/Renderer.hpp"

// Register factory: Renderer
static WatermarkingProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> SharedPtr<Renderer> { return std::make_shared<Renderer>(project); }
};

Renderer& Renderer::Get(WatermarkingProject& project)
{
    return project.AttachedObjects::Get<Renderer>(factoryKey);
}

Renderer const& Renderer::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

Renderer::Renderer(WatermarkingProject& project)
    : m_project(project)
{
}

void Renderer::Render(Graphics& gfx)
{
    for (auto const& task : m_tasks) {
        task.Execute(gfx);
    }
}

void Renderer::Accept(Task task)
{
    m_tasks.push_back(task);
}

std::vector<Task>& Renderer::GetTasks()
{
    return m_tasks;
}
