#include <utility>

#include "gfx/Technique.hpp"
#include "gfx/bindable/UniformBuffer.hpp"

Technique::Technique(std::string id)
    : m_id(id)
{
}

void Technique::Submit(Renderer& renderer, DrawableBase& drawable)
{
    for (auto& step : m_steps) {
        step.Submit(renderer, drawable);
    }
}

void Technique::AddBindStep(BindStep step)
{
    m_steps.push_back(std::move(step));
}

void Technique::UpdateUniform(Graphics& gfx, std::string const& id, UniformBlock const& block)
{
    auto const& bindings = m_steps.front().bindables;
    for (auto it = bindings.begin(); it != bindings.end(); it++) {
        Bind::UniformBuffer* buf = dynamic_cast<Bind::UniformBuffer*>(it->get());
        if (buf != nullptr) {
            if (buf->Id() == id) {
                buf->Update(gfx, block);
            }
        }
    }
}

std::string Technique::GetID() const
{
    return m_id;
}
