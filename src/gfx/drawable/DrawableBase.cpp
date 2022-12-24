#include <utility>

#include "gfx/DrawTask.hpp"
#include "gfx/Renderer.hpp"
#include "gfx/bindable/Bindable.hpp"
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

void DrawableBase::AddTechnique(Technique tech)
{
    m_techs.push_back(std::move(tech));
}

void DrawableBase::Submit(Renderer& renderer)
{
    for (auto& tech : m_techs) {
        tech.Submit(renderer, *this);
    }
};

void DrawableBase::Bind(Graphics& gfx) const
{
    for (auto const& b : m_binds) {
        b->Bind(gfx);
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
