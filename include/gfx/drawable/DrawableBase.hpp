#ifndef DRAWABLE_BASE_H
#define DRAWABLE_BASE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "gfx/Technique.hpp"
#include "gfx/UniformBlock.hpp"

class Renderer;
class Graphics;

namespace Bind
{
    class Bindable;
};

class DrawableBase
{
public:
    DrawableBase();
    DrawableBase(DrawableBase const&) = delete;
    virtual ~DrawableBase() = default;
    virtual void AddBind(std::shared_ptr<Bind::Bindable> bind);
    void AddTechnique(Technique tech);
    void Submit(Renderer& renderer);
    void Bind(Graphics& gfx) const;

    virtual void Draw(Graphics& gfx) const = 0;
    void SetVisible(bool visible);
    bool IsVisible() const;

protected:
    bool m_isVisible;
    std::vector<Technique> m_techs;
    std::vector<std::shared_ptr<Bind::Bindable>> m_binds;

    std::string m_id;
};

#endif
