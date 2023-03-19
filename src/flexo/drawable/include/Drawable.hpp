#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "gfx/BindStep.hpp"
#include "gfx/UniformBlock.hpp"
#include "gfx/DrawableBase.hpp"

class Graphics;

namespace Bind
{
    class Bindable;
}

class Drawable : public DrawableBase
{
public:
    Drawable();
    Drawable(Drawable const&) = delete;
    virtual ~Drawable() = default;
    virtual void AddBind(std::shared_ptr<Bind::Bindable> bind);
    void Submit(Renderer& renderer);
    void SetTransform(glm::mat4 transform);
    void AddBindStep(BindStep step);
    void UpdateUniformBuffers(Graphics& gfx) const;
    void SetVisible(bool visible);
    bool IsVisible() const;
    void Bind(Graphics& gfx) const;

    virtual void Draw(Graphics& gfx) const override;

protected:
    bool m_isVisible;
    glm::mat4 m_transform;
    std::vector<BindStep> m_steps;
    std::vector<std::shared_ptr<Bind::Bindable>> m_binds;
    std::unordered_map<std::string, UniformBlock> m_ubs;

    std::string m_id;
    unsigned int m_vertCount;
};

#endif
