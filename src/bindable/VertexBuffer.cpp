#include "bindable/VertexBuffer.hpp"
#include "Vertex.hpp"

void VertexBuffer::Bind(Graphics& gfx)
{
    int const numBuffers = offsets_.size();
    std::vector<GLuint> buffers(numBuffers, id_);
    gfx.SetVertexBuffers(0, numBuffers, buffers.data(), offsets_.data());
}

GLuint VertexBuffer::GetCount() const
{
    return count_;
}

template <>
void VertexBuffer::SetOffsets<VertexPN>()
{
    offsets_ = { offsetof(VertexPN, position), offsetof(VertexPN, normal) };
}

template <>
void VertexBuffer::SetOffsets<VertexPNT>()
{
    offsets_ = { offsetof(VertexPNT, position), offsetof(VertexPNT, normal), offsetof(VertexPNT, texcoord) };
}

template <>
void VertexBuffer::SetOffsets<VertexPNC>()
{
    offsets_ = { offsetof(VertexPNC, position), offsetof(VertexPNC, normal), offsetof(VertexPNC, color) };
}
