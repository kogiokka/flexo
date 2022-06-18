#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <vector>

#include "Graphics.hpp"
#include "Vertex.hpp"
#include "bindable/Bindable.hpp"

class VertexBuffer : public Bindable
{
protected:
    GLuint id_;
    std::vector<GLintptr> offsets_;

public:
    template <typename T>
    VertexBuffer(Graphics& gfx, std::vector<T> const& vertices);
    void Bind(Graphics& gfx) override;

protected:
    template <typename T>
    void setOffsets();
};

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

    setOffsets<T>();

    data.mem = vertices.data();
    gfx.CreateBuffer(id_, desc, data);
}

template <>
void VertexBuffer::setOffsets<VertexPN>()
{
    offsets_ = { offsetof(VertexPN, position), offsetof(VertexPN, normal) };
}

template <>
void VertexBuffer::setOffsets<VertexPNT>()
{
    offsets_ = { offsetof(VertexPNT, position), offsetof(VertexPNT, normal), offsetof(VertexPNT, texcoord) };
}

template <>
void VertexBuffer::setOffsets<VertexPNC>()
{
    offsets_ = { offsetof(VertexPNC, position), offsetof(VertexPNC, normal), offsetof(VertexPNC, color) };
}

#endif
