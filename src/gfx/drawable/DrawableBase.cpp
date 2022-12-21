#include <utility>

#include "gfx/DrawTask.hpp"
#include "gfx/Renderer.hpp"
#include "gfx/bindable/UniformBuffer.hpp"
#include "gfx/drawable/DrawableBase.hpp"

DrawableBase::DrawableBase()
    : m_isVisible(true)
{
}

void DrawableBase::AddBind(std::shared_ptr<Bind::Bindable> bind)
{
    m_binds.push_back(std::move(bind));
}

void DrawableBase::AddBindStep(BindStep step)
{
    m_steps.push_back(std::move(step));
}

void DrawableBase::Submit(Renderer& renderer)
{
    for (auto& step : m_steps) {
        step.Submit(renderer, *this);
    }
};

void DrawableBase::Bind(Graphics& gfx) const
{
    for (auto const& b : m_binds) {
        b->Bind(gfx);
    }
}

void DrawableBase::UpdateUniformBuffers(Graphics& gfx) const
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

void DrawableBase::SetVisible(bool visible)
{
    m_isVisible = visible;
}

bool DrawableBase::IsVisible() const
{
    return m_isVisible;
}
