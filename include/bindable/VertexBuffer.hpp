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
    GLuint startAttrib_;
    std::vector<GLintptr> offsets_;
    std::vector<GLsizei> strides_;

public:
    template <typename T>
    VertexBuffer(Graphics& gfx, std::vector<T> const& vertices, GLuint startAttrib = 0);
    void Bind(Graphics& gfx) override;
    GLuint GetCount() const;

protected:
    template <typename T>
    void SetOffsets();
};

template <typename T>
VertexBuffer::VertexBuffer(Graphics& gfx, std::vector<T> const& vertices, GLuint startAttrib)
    : id_(0)
    , count_(vertices.size())
    , startAttrib_(startAttrib)
{
    BufferDesc desc;
    BufferData data;

    desc.target = GL_ARRAY_BUFFER;
    desc.usage = GL_STATIC_DRAW;
    desc.byteWidth = sizeof(T) * vertices.size();
    desc.stride = sizeof(T);
    data.mem = vertices.data();

    SetOffsets<T>();

    gfx.CreateBuffer(id_, desc, data);
}

template <>
void VertexBuffer::SetOffsets<VertexPN>();
template <>
void VertexBuffer::SetOffsets<VertexPNT>();
template <>
void VertexBuffer::SetOffsets<VertexPNC>();
template <>
void VertexBuffer::SetOffsets<glm::vec3>();
template <>
void VertexBuffer::SetOffsets<glm::vec2>();

#endif
