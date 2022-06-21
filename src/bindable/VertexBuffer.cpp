#include "bindable/VertexBuffer.hpp"
#include "Vertex.hpp"

#include "common/Logger.hpp"

void VertexBuffer::Bind(Graphics& gfx)
{
    int const numBuffers = offsets_.size();
    gfx.SetVertexBuffers(startAttrib_, numBuffers, std::vector<GLuint>(numBuffers, id_).data(), offsets_.data(), strides_.data());
}

GLuint VertexBuffer::GetCount() const
{
    return count_;
}

template <>
void VertexBuffer::SetOffsets<VertexPN>()
{
    offsets_ = { offsetof(VertexPN, position), offsetof(VertexPN, normal) };
    strides_ = std::vector<GLsizei>(2, sizeof(VertexPN));
}

template <>
void VertexBuffer::SetOffsets<VertexPNT>()
{
    offsets_ = { offsetof(VertexPNT, position), offsetof(VertexPNT, normal), offsetof(VertexPNT, texcoord) };
    strides_ = std::vector<GLsizei>(3, sizeof(VertexPNT));
}

template <>
void VertexBuffer::SetOffsets<VertexPNC>()
{
    offsets_ = { offsetof(VertexPNC, position), offsetof(VertexPNC, normal), offsetof(VertexPNC, color) };
    strides_ = std::vector<GLsizei>(3, sizeof(VertexPNC));
}

template <>
void VertexBuffer::SetOffsets<glm::vec3>()
{
    offsets_ = { 0 };
    strides_ = std::vector<GLsizei>(1, sizeof(glm::vec3));
}

template <>
void VertexBuffer::SetOffsets<glm::vec2>()
{
    offsets_ = { 0 };
    strides_ = std::vector<GLsizei>(1, sizeof(glm::vec2));
}
