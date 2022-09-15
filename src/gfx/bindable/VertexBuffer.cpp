#include "gfx/bindable/VertexBuffer.hpp"
#include "Vertex.hpp"

namespace Bind
{
    VertexBuffer::~VertexBuffer()
    {
    }

    void VertexBuffer::Bind(Graphics& gfx)
    {
        unsigned int const offset = 0;
        gfx.SetVertexBuffers(m_startAttrib, 1, m_buffer.GetAddressOf(), &m_stride, &offset);
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
