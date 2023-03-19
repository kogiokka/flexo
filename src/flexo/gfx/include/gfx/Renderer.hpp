#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <vector>

#include "DrawTask.hpp"
#include "gfx/Graphics.hpp"

class Renderer
{
public:
    Renderer();
    void Render(Graphics& gfx);
    void Accept(DrawTask task);
    void Clear();

private:
    std::vector<DrawTask> m_tasks;
};

#endif
