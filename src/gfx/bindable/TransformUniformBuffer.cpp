#include "gfx/bindable/TransformUniformBuffer.hpp"

namespace Bind
{
    TransformUniformBuffer::TransformUniformBuffer(Graphics& gfx, glm::mat4 transform)
        : m_mat(transform)
        , m_buffer(nullptr)
    {
        m_ub.AddElement(UniformBlock::Type::mat4, "model");
        m_ub.AddElement(UniformBlock::Type::mat4, "viewProj");
        m_ub.FinalizeLayout();
        m_buffer = std::make_unique<UniformBuffer>(gfx, m_ub);
    }

    void TransformUniformBuffer::Bind(Graphics& gfx)
    {
        m_ub.Assign("model", m_mat);
        m_ub.Assign("viewProj", gfx.GetViewProjectionMatrix());
        m_buffer->Update(gfx, m_ub);
        m_buffer->Bind(gfx);
    }
}
