#include "bindable/IndexBuffer.hpp"

namespace Bind
{
    IndexBuffer::IndexBuffer(Graphics& gfx, std::vector<unsigned int> const& indices)
        : Bindable(gfx)
        , m_count(indices.size())
    {
        GLWRBufferDesc desc;
        GLWRResourceData data;

        desc.target = GL_ELEMENT_ARRAY_BUFFER;
        desc.usage = GL_STATIC_DRAW;
        desc.byteWidth = sizeof(unsigned int) * m_count;
        desc.stride = sizeof(unsigned int);
        data.mem = indices.data();

        m_gfx->CreateBuffer(&desc, &data, &m_buffer);
    }

    IndexBuffer::~IndexBuffer()
    {
    }

    void IndexBuffer::Bind()
    {
        m_gfx->SetIndexBuffer(GL_UNSIGNED_INT, 0, m_buffer.Get());
    }

    GLuint IndexBuffer::GetCount() const
    {
        return m_count;
    }
}
