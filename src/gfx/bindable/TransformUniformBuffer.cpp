#include "gfx/bindable/TransformUniformBuffer.hpp"

namespace Bind
{
    TransformUniformBuffer::TransformUniformBuffer(Graphics& gfx, glm::mat4 transform)
        : m_mat(transform)
        , m_buffer(gfx, TransformMatrices { m_mat, gfx.GetViewProjectionMatrix() })
    {
    }

    void TransformUniformBuffer::Bind(Graphics& gfx)
    {
        m_buffer.Update<TransformMatrices>(gfx, { m_mat, gfx.GetViewProjectionMatrix() });
        m_buffer.Bind(gfx);
    }
}
