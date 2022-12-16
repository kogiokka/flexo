#ifndef DRAWABLE_BASE_H
#define DRAWABLE_BASE_H

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "gfx/Graphics.hpp"
#include "gfx/Task.hpp"
#include "gfx/bindable/Bindable.hpp"

class Renderer;

class DrawableBase
{
public:
    DrawableBase() = default;
    DrawableBase(DrawableBase const&) = delete;
    virtual ~DrawableBase() = default;
    virtual void AddBind(std::shared_ptr<Bind::Bindable> bind) = 0;
    void Submit(Renderer& renderer) const;
    void AddTask(Task task);

    virtual void Update(Graphics& gfx) = 0;
    virtual void Bind(Graphics& gfx) const = 0;
    void SetVisible(bool visible);
    bool IsVisible() const;

    void SetID(std::string id);
    std::string GetID() const;

protected:
    bool m_isVisible;
    std::vector<Task> m_tasks;
    std::vector<std::shared_ptr<Bind::Bindable>> m_binds;

    std::string m_id;
};

#endif
