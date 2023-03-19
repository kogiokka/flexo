#include "gfx/BindStep.hpp"
#include "gfx/DrawTask.hpp"
#include "gfx/DrawableBase.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/Renderer.hpp"

void BindStep::AddBindable(std::shared_ptr<Bind::Bindable> bind)
{
    bindables.push_back(bind);
}

void BindStep::Submit(Renderer& renderer, DrawableBase& drawable)
{
    renderer.Accept(DrawTask { this, &drawable });
}

void BindStep::Bind(Graphics& gfx) const
{
    for (auto const& b : bindables) {
        b->Bind(gfx);
    }
}
