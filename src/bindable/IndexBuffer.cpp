#include "bindable/IndexBuffer.hpp"

namespace Bind
{
    IndexBuffer::IndexBuffer(Graphics& gfx, std::vector<unsigned int> const& indices)
        : Bindable(gfx)
        , m_id(0)
        , m_count(indices.size())
    {
        BufferDesc desc;
        BufferData data;

        desc.target = GL_ELEMENT_ARRAY_BUFFER;
        desc.usage = GL_STATIC_DRAW;
        desc.byteWidth = sizeof(unsigned int) * m_count;
        desc.stride = sizeof(unsigned int);
        data.mem = indices.data();

        m_gfx->CreateBuffer(m_id, desc, data);
    }

    IndexBuffer::~IndexBuffer() { m_gfx->DeleteBuffer(m_id); }

    void IndexBuffer::Bind() { m_gfx->SetIndexBuffer(m_id, GL_UNSIGNED_INT, 0); }

    GLuint IndexBuffer::GetCount() const { return m_count; }
}
