#include "gfx/bindable/VertexBuffer.hpp"
#include "Vertex.hpp"

namespace Bind
{
    VertexBuffer::~VertexBuffer()
    {
    }

    void VertexBuffer::Bind()
    {
        unsigned int const offset = 0;
        m_gfx->SetVertexBuffers(m_startAttrib, 1, &m_buffer, &m_stride, &offset);
    }

    unsigned int VertexBuffer::GetStartAttrib() const
    {
        return m_startAttrib;
    }

    unsigned int VertexBuffer::GetCount() const
    {
        return m_count;
    }
}
