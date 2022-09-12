#include "gfx/bindable/VertexBuffer.hpp"
#include "Vertex.hpp"

namespace Bind
{
    VertexBuffer::~VertexBuffer()
    {
    }

    void VertexBuffer::Bind()
    {
        const GLintptr offset = 0;
        m_gfx->SetVertexBuffers(m_startAttrib, 1, &m_buffer, &m_stride, &offset);
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
