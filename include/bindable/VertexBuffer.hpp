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
    GLuint count_;
    std::vector<GLintptr> offsets_;

public:
    template <typename T>
    VertexBuffer(Graphics& gfx, std::vector<T> const& vertices);
    void Bind(Graphics& gfx) override;
    GLuint GetCount() const;

protected:
    template <typename T>
    void setOffsets();
};

template <typename T>
VertexBuffer::VertexBuffer(Graphics& gfx, std::vector<T> const& vertices)
    : id_(0)
    , count_(vertices.size())
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
void VertexBuffer::setOffsets<VertexPN>();
template <>
void VertexBuffer::setOffsets<VertexPNT>();
template <>
void VertexBuffer::setOffsets<VertexPNC>();

#endif
