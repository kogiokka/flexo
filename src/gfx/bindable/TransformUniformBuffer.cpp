#include "gfx/bindable/TransformUniformBuffer.hpp"

namespace Bind
{
    TransformUniformBuffer::TransformUniformBuffer(Graphics& gfx, glm::mat4 transform)
        : m_buffer(gfx)
        , m_mat(transform)
    {
    }

    void TransformUniformBuffer::Bind(Graphics& gfx)
    {
        m_buffer.Update(gfx, { m_mat, gfx.GetViewProjectionMatrix() });
        m_buffer.Bind(gfx);
    }
}
