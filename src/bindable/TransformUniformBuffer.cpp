#include "bindable/TransformUniformBuffer.hpp"

namespace Bind
{
    TransformUniformBuffer::TransformUniformBuffer(Graphics& gfx, DrawableBase const& parent)
        : Bindable(gfx)
        , m_buffer(gfx)
        , m_parent(parent)
    {
    }

    void TransformUniformBuffer::Bind()
    {
        m_buffer.Update({ m_parent.GetTransformMatrix(), m_gfx->GetViewProjectionMatrix() });
        m_buffer.Bind();
    }
}
