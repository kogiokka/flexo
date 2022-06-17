#include <type_traits>

#include "Vertex.hpp"
#include "bindable/VertexBuffer.hpp"

template <typename T>
VertexBuffer::VertexBuffer(Graphics& gfx, std::vector<T> const& vertices)
    : id_(0)
{
    BufferDesc desc;
    BufferData data;

    desc.target = GL_ARRAY_BUFFER;
    desc.usage = GL_STATIC_DRAW;
    desc.byteWidth = sizeof(T) * vertices.size();
    desc.stride = sizeof(T);
    data.mem = vertices.data();
    gfx.CreateBuffer(id_, desc, data);

    if (std::is_same_v<T, VertexPN>) {
        GLsizei const stride = sizeof(VertexPN);
        offsets_ = { offsetof(VertexPN, position), offsetof(VertexPN, normal) };
        strides_ = { stride, stride };
    } else if (std::is_same_v<T, VertexPNT>) {
        GLsizei const stride = sizeof(VertexPNT);
        offsets_ = { offsetof(VertexPNT, position), offsetof(VertexPNT, normal), offsetof(VertexPNT, texcoord) };
        strides_ = { stride, stride, stride };
    } else if (std::is_same_v<T, VertexPNC>) {
        GLsizei const stride = sizeof(VertexPNC);
        offsets_ = { offsetof(VertexPNC, position), offsetof(VertexPNC, normal), offsetof(VertexPNC, color) };
        strides_ = { stride, stride, stride };
    }
}

void VertexBuffer::Bind(Graphics& gfx)
{
    int const numBuffers = offsets_.size();
    std::vector<GLuint> buffers(numBuffers, id_);
    gfx.SetVertexBuffers(0, numBuffers, buffers.data(), offsets_.data(), strides_.data());
}
