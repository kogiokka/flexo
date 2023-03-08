#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Project.hpp"
#include "gfx/Renderer.hpp"
#include "util/Logger.h"

// Register factory: Renderer
static FlexoProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](FlexoProject& project) -> SharedPtr<Renderer> { return std::make_shared<Renderer>(project); }
};

Renderer& Renderer::Get(FlexoProject& project)
{
    return project.AttachedObjects::Get<Renderer>(factoryKey);
}

Renderer const& Renderer::Get(FlexoProject const& project)
{
    return Get(const_cast<FlexoProject&>(project));
}

Renderer::Renderer(FlexoProject& project)
    : m_project(project)
{
}

void Renderer::Render(Graphics& gfx)
{
    for (auto& task : m_tasks) {
        task.Execute(gfx);
    }
}

void Renderer::Accept(DrawTask task)
{
    m_tasks.push_back(task);
}

void Renderer::Clear()
{
    m_tasks.clear();
}
