#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <vector>

#include "Attachable.hpp"
#include "DrawTask.hpp"
#include "Mesh.hpp"
#include "gfx/Graphics.hpp"

class FlexoProject;

class Renderer : public AttachableBase
{
public:
    static Renderer& Get(FlexoProject& project);
    static Renderer const& Get(FlexoProject const& project);

    Renderer(FlexoProject& project);
    void Render(Graphics& gfx);
    void Accept(DrawTask task);
    void Clear();

private:
    std::vector<DrawTask> m_tasks;
    FlexoProject& m_project;
};

#endif
