#include "gfx/drawable/IndexedDrawable.hpp"

IndexedDrawable::IndexedDrawable()
{
    m_buffer = nullptr;
}

void IndexedDrawable::Bind(Graphics& gfx) const
{
    if (m_isVisible) {
        for (auto& b : m_binds) {
            b->Bind();
        }
        gfx.DrawIndexed(m_buffer->GetCount());
    }
}

void IndexedDrawable::Update(Graphics&)
{
}

void IndexedDrawable::AddBind(std::shared_ptr<Bind::Bindable> bind)
{
    if (m_buffer == nullptr) {
        m_buffer = dynamic_cast<Bind::IndexBuffer*>(bind.get());
    }
    m_binds.push_back(bind);
}
