#ifndef DRAWABLE_BASE_H
#define DRAWABLE_BASE_H

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

class DrawableBase
{
protected:
    bool m_isVisible;
    std::string m_name;
    std::vector<std::shared_ptr<Bind::Bindable>> m_binds;

public:
    DrawableBase() = default;
    DrawableBase(DrawableBase const&) = delete;
    virtual ~DrawableBase() = default;

    virtual void Draw(Graphics& gfx) const = 0;
    virtual glm::mat4 GetTransformMatrix() const = 0;
    virtual void Update(Graphics& gfx) = 0;
    virtual void AddBind(std::shared_ptr<Bind::Bindable> bind) = 0;
    virtual std::string GetName() const = 0;
    void SetVisible(bool visible)
    {
        m_isVisible = visible;
    }
    bool IsVisible() const
    {
        return m_isVisible;
    }
    void SetName(std::string const& name)
    {
        m_name = name;
    }
    std::string GetName() const
    {
        return m_name;
    }
};

#endif
