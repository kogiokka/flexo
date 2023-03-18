#include "gfx/bindable/VertexBuffer.hpp"

namespace Bind
{
    VertexBuffer::VertexBuffer(Graphics& gfx, std::vector<Vertex> const& vertices, unsigned int startAttrib)
        : m_startAttrib(startAttrib)
        , m_stride(vertices.front().GetStride())
        , m_count(vertices.size())
    {
        GLWRBufferDesc desc;
        GLWRResourceData data;

        desc.type = GLWRResourceType_Buffer;
        desc.usage = GL_DYNAMIC_DRAW;
        desc.byteWidth = m_count * m_stride;
        desc.stride = m_stride;

        auto buf = GenBuffer(vertices);
        data.mem = buf.data();

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

    void VertexBuffer::Update(Graphics& gfx, std::vector<Vertex> const& vertices)
    {
        GLWRMappedSubresource mem;
        gfx.Map(m_buffer.Get(), GLWRMapPermission_WriteOnly, &mem);
        std::memcpy(mem.data, GenBuffer(vertices).data(), vertices.size() * vertices.front().GetStride());
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

    std::vector<unsigned char> VertexBuffer::GenBuffer(std::vector<Vertex> const& vertices) const
    {
        std::vector<unsigned char> buf;
        buf.resize(m_count * m_stride);
        unsigned int offset = 0;
        for (auto const& v : vertices) {
            std::memcpy(buf.data() + offset, v.GetData(), m_stride);
            offset += m_stride;
        }
        return buf;
    }
}
