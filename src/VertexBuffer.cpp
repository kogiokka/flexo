#include "VertexBuffer.hpp"

template <typename T>
VertexBuffer::VertexBuffer(Graphics& gfx, std::vector<T> const& vertices)
    : id_(0)
    , stride_(sizeof(T))
{
    BufferDesc desc;
    BufferData data;

    desc.target = GL_ARRAY_BUFFER;
    desc.usage = GL_STATIC_DRAW;
    desc.byteWidth = sizeof(T) * vertices.size();
    data.mem = vertices.data();

    gfx.CreateBuffer(id_, desc, data);
}

void VertexBuffer::Bind(Graphics& gfx)
{
    gfx.SetVertexBuffer(id_);
}
