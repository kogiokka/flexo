#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Project.hpp"
#include "Renderer.hpp"
#include "World.hpp"
#include "common/Logger.hpp"

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

BoundingBox Renderer::CalculateBoundingBox(std::vector<glm::vec3> positions)
{
    const float FLOAT_MAX = std::numeric_limits<float>::max();
    glm::vec3 min = { FLOAT_MAX, FLOAT_MAX, FLOAT_MAX };
    glm::vec3 max = { -FLOAT_MAX, -FLOAT_MAX, -FLOAT_MAX };

    for (auto const& p : positions) {
        if (p.x > max.x) {
            max.x = p.x;
        }
        if (p.y > max.y) {
            max.y = p.y;
        }
        if (p.z > max.z) {
            max.z = p.z;
        }
        if (p.x < min.x) {
            min.x = p.x;
        }
        if (p.y < min.y) {
            min.y = p.y;
        }
        if (p.z < min.z) {
            min.z = p.z;
        }
    }
    return { max, min };
}
