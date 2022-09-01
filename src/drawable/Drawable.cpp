#include "drawable/Drawable.hpp"

Drawable::Drawable()
{
    m_buffer = nullptr;
}

void Drawable::Bind(Graphics& gfx) const
{
    if (m_isVisible) {
        for (auto& b : m_binds) {
            b->Bind();
        }
        gfx.Draw(m_buffer->GetCount());
    }
}

void Drawable::Update(Graphics&)
{
}

void Drawable::AddBind(std::shared_ptr<Bind::Bindable> bind)
{
    if (m_buffer == nullptr) {
        m_buffer = dynamic_cast<Bind::VertexBuffer*>(bind.get());
    }
    m_binds.push_back(bind);
}

