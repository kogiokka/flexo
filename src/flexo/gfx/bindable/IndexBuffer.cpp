#include "gfx/bindable/IndexBuffer.hpp"

namespace Bind
{
    IndexBuffer::IndexBuffer(Graphics& gfx, std::vector<unsigned int> const& indices)
        : m_count(indices.size())
    {
        GLWRBufferDesc desc;
        GLWRResourceData data;

        desc.type = GLWRResourceType_IndexBuffer;
        desc.usage = GL_STATIC_DRAW;
        desc.byteWidth = sizeof(unsigned int) * m_count;
        desc.stride = sizeof(unsigned int);
        data.mem = indices.data();

        gfx.CreateBuffer(&desc, &data, &m_buffer);
    }

    IndexBuffer::~IndexBuffer()
    {
    }

    void IndexBuffer::Bind(Graphics& gfx)
    {
        gfx.SetIndexBuffer(m_buffer.Get(), GLWRFormat_Uint, 0);
    }

    GLuint IndexBuffer::GetCount() const
    {
        return m_count;
    }
}
