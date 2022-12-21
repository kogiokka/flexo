#ifndef DRAWABLE_BASE_H
#define DRAWABLE_BASE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "gfx/BindStep.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/UniformBlock.hpp"
#include "gfx/bindable/Bindable.hpp"

class Renderer;

class DrawableBase
{
public:
    DrawableBase();
    DrawableBase(DrawableBase const&) = delete;
    virtual ~DrawableBase() = default;
    virtual void AddBind(std::shared_ptr<Bind::Bindable> bind);
    void Submit(Renderer& renderer);
    void AddBindStep(BindStep step);
    void UpdateUniformBuffers(Graphics& gfx) const;
    void Bind(Graphics& gfx) const;

    virtual void Update(Graphics& gfx) = 0;
    virtual void Draw(Graphics& gfx) const = 0;
    void SetVisible(bool visible);
    bool IsVisible() const;

protected:
    bool m_isVisible;
    std::vector<BindStep> m_steps;
    std::vector<std::shared_ptr<Bind::Bindable>> m_binds;
    std::unordered_map<std::string, UniformBlock> m_ubs;

    std::string m_id;
};

#endif

