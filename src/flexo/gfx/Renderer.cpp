#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "gfx/Renderer.hpp"
#include "log/Logger.h"

Renderer::Renderer()
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
