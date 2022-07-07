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
}






