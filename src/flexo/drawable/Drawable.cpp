#include <utility>

#include "Drawable.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/bindable/UniformBuffer.hpp"

Drawable::Drawable()
    : m_isVisible(true)
    , m_transform(glm::mat4(1.0f))
{
}

void Drawable::Draw(Graphics& gfx) const
{
    if (!IsVisible()) {
        return;
    }

    UpdateUniformBuffers(gfx);
    for (auto const& b : m_binds) {
        b->Bind(gfx);
    }

    gfx.Draw(m_vertCount);
}

void Drawable::AddBind(std::shared_ptr<Bind::Bindable> bind)
{
    m_binds.push_back(std::move(bind));
}

void Drawable::AddBindStep(BindStep step)
{
    m_steps.push_back(std::move(step));
}

void Drawable::Submit(Renderer& renderer)
{
    for (auto& step : m_steps) {
        step.Submit(renderer, *this);
    }
}

void Drawable::SetTransform(glm::mat4 transform)
{
    m_transform = transform;
}

void Drawable::UpdateUniformBuffers(Graphics& gfx) const
{
    // FIXME Better solution
    auto const& bindings = m_steps.front().bindables;
    for (auto it = bindings.begin(); it != bindings.end(); it++) {
        Bind::UniformBuffer* buf = dynamic_cast<Bind::UniformBuffer*>(it->get());
        if (buf != nullptr) {
            auto it = m_ubs.find(buf->Id());
            if (it != m_ubs.end()) {
                buf->Update(gfx, it->second);
            }
        }
    }
}

void Drawable::SetVisible(bool visible)
{
    m_isVisible = visible;
}

bool Drawable::IsVisible() const
{
    return m_isVisible;
}
