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

void Renderer::SetCameraView(BoundingBox const& box)
{
    auto const& [max, min] = box;
    glm::vec3 center = (max + min) * 0.5f;

    auto& camera = Graphics::Get(m_project).GetCamera();
    camera.center = center;
    Logger::info("Camera looking at: %s", glm::to_string(center).c_str());

    camera.UpdateViewCoord();

    glm::vec3 diff = max - min;
    float size = diff.x;
    if (size < diff.y) {
        size = diff.y;
    }
    if (size < diff.z) {
        size = diff.z;
    }
    camera.volumeSize = size;
}

std::vector<Task>& Renderer::GetTasks()
{
    return m_tasks;
}
