#include "bindable/VertexBuffer.hpp"
#include "Vertex.hpp"

namespace Bind
{
    VertexBuffer::~VertexBuffer()
    {
        m_gfx->DeleteBuffer(m_id);
    }

    void VertexBuffer::Bind()
    {
        m_gfx->SetVertexBuffers(m_startAttrib, m_numAttrs, std::vector<GLuint>(m_numAttrs, m_id).data(), m_offsets.data(),
                               m_strides.data());
    }

    GLuint VertexBuffer::GetStartAttrib() const
    {
        return m_startAttrib;
    }

    GLuint VertexBuffer::GetCount() const
    {
        return m_count;
    }

    template <>
    void VertexBuffer::SetAttribMemLayout<VertexPN>()
    {
        m_numAttrs = 2;
        m_offsets = { offsetof(VertexPN, position), offsetof(VertexPN, normal) };
        m_strides = std::vector<GLsizei>(m_numAttrs, sizeof(VertexPN));
    }

    template <>
    void VertexBuffer::SetAttribMemLayout<VertexPNT>()
    {
        m_numAttrs = 3;
        m_offsets = { offsetof(VertexPNT, position), offsetof(VertexPNT, normal), offsetof(VertexPNT, texcoord) };
        m_strides = std::vector<GLsizei>(m_numAttrs, sizeof(VertexPNT));
    }

    template <>
    void VertexBuffer::SetAttribMemLayout<VertexPNC>()
    {
        m_numAttrs = 3;
        m_offsets = { offsetof(VertexPNC, position), offsetof(VertexPNC, normal), offsetof(VertexPNC, color) };
        m_strides = std::vector<GLsizei>(m_numAttrs, sizeof(VertexPNC));
    }

    template <>
    void VertexBuffer::SetAttribMemLayout<glm::vec3>()
    {
        m_numAttrs = 1;
        m_offsets = { 0 };
        m_strides = std::vector<GLsizei>(m_numAttrs, sizeof(glm::vec3));
    }

    template <>
    void VertexBuffer::SetAttribMemLayout<glm::vec2>()
    {
        m_numAttrs = 1;
        m_offsets = { 0 };
        m_strides = std::vector<GLsizei>(m_numAttrs, sizeof(glm::vec2));
    }
}
