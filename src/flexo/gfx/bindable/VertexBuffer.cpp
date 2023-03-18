#include "gfx/bindable/VertexBuffer.hpp"

namespace Bind
{
    VertexBuffer::VertexBuffer(Graphics& gfx, VertexArray const& vertices, unsigned int startAttrib)
        : m_startAttrib(startAttrib)
        , m_stride(vertices.GetStride())
        , m_count(vertices.GetCount())
    {
        GLWRBufferDesc desc;
        GLWRResourceData data;

        desc.type = GLWRResourceType_Buffer;
        desc.usage = GL_DYNAMIC_DRAW;
        desc.byteWidth = m_count * m_stride;
        desc.stride = m_stride;
        data.mem = vertices.GetData();

        gfx.CreateBuffer(&desc, &data, &m_buffer);
    }

    VertexBuffer::~VertexBuffer()
    {
    }

    void VertexBuffer::Bind(Graphics& gfx)
    {
        unsigned int const offset = 0;
        gfx.SetVertexBuffers(m_startAttrib, 1, m_buffer.GetAddressOf(), &m_stride, &offset);
    }

    void VertexBuffer::Update(Graphics& gfx, VertexArray const& vertices)
    {
        GLWRMappedSubresource mem;
        gfx.Map(m_buffer.Get(), GLWRMapPermission_WriteOnly, &mem);
        std::memcpy(mem.data, vertices.GetData(), vertices.GetCount() * vertices.GetStride());
        gfx.Unmap(m_buffer.Get());
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
