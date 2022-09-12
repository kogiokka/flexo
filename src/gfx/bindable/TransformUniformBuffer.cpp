#include "gfx/bindable/TransformUniformBuffer.hpp"

namespace Bind
{
    TransformUniformBuffer::TransformUniformBuffer(Graphics& gfx, glm::mat4 transform)
        : Bindable(gfx)
        , m_buffer(gfx)
        , m_mat(transform)
    {
    }

    void TransformUniformBuffer::Bind()
    {
        m_buffer.Update({ m_mat, m_gfx->GetViewProjectionMatrix() });
        m_buffer.Bind();
    }
}
