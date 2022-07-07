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
        const GLintptr offset = 0;
        m_gfx->SetVertexBuffers(m_startAttrib, 1, &m_id, &offset, &m_stride);
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
