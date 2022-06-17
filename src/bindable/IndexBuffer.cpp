#include "bindable/IndexBuffer.hpp"

IndexBuffer::IndexBuffer(Graphics& gfx, std::vector<unsigned int> const& indices)
    : id_(0)
    , count_(indices.size())
{
    BufferDesc desc;
    BufferData data;

    desc.target = GL_ELEMENT_ARRAY_BUFFER;
    desc.usage = GL_STATIC_DRAW;
    desc.byteWidth = sizeof(unsigned int) * count_;
    desc.stride = sizeof(unsigned int);
    data.mem = indices.data();

    gfx.CreateBuffer(id_, desc, data);
}

void IndexBuffer::Bind(Graphics& gfx)
{
    gfx.SetIndexBuffer(id_, GL_UNSIGNED_INT, 0);
}

GLuint IndexBuffer::GetCount() const
{
    return count_;
}
