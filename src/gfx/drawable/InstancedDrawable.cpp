#include "gfx/drawable/InstancedDrawable.hpp"

InstancedDrawable::InstancedDrawable()
{
    m_instance = nullptr;
    m_perInstanceData = nullptr;
}

void InstancedDrawable::Bind(Graphics& gfx) const
{
    if (m_isVisible) {
        for (auto& b : m_binds) {
            b->Bind(gfx);
        }
        gfx.DrawInstanced(m_instance->GetCount(), m_perInstanceData->GetCount());
    }
}

void InstancedDrawable::Update(Graphics&)
{
}

void InstancedDrawable::AddBind(std::shared_ptr<Bind::Bindable> bind)
{
    // FIXME: Make it reliable
    if (m_instance == nullptr) {
        m_instance = dynamic_cast<Bind::VertexBuffer*>(bind.get());
    } else if (m_perInstanceData == nullptr) {
        m_perInstanceData = dynamic_cast<Bind::VertexBuffer*>(bind.get());
    }

    m_binds.push_back(bind);
}
