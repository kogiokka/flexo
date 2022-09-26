#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <vector>

#include "Attachable.hpp"
#include "Mesh.hpp"
#include "Task.hpp"
#include "gfx/Graphics.hpp"

class WatermarkingProject;

class Renderer : public AttachableBase
{
public:
    static Renderer& Get(WatermarkingProject& project);
    static Renderer const& Get(WatermarkingProject const& project);

    Renderer(WatermarkingProject& project);
    void Render(Graphics& gfx);
    void Accept(Task task);
    void Clear();

private:
    std::vector<Task> m_tasks;
    WatermarkingProject& m_project;
};

#endif
