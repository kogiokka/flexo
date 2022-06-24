#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <vector>

#include "Graphics.hpp"
#include "Vertex.hpp"
#include "bindable/Bindable.hpp"

namespace Bind
{
    class VertexBuffer : public Bindable
    {
    protected:
        GLuint id_;
        GLuint count_;
        GLuint startAttrib_;
        int numAttrs_;
        std::vector<GLintptr> offsets_;
        std::vector<GLsizei> strides_;

    public:
        template <typename T>
        VertexBuffer(Graphics& gfx, std::vector<T> const& vertices, GLuint startAttrib = 0);
        ~VertexBuffer();
        void Bind() override;
        template <typename T>
        void Update(std::vector<T> const& vertices);
        GLuint GetStartAttrib() const;
        GLuint GetCount() const;

    protected:
        template <typename T>
        void SetAttribMemLayout();
    };

    template <typename T>
    VertexBuffer::VertexBuffer(Graphics& gfx, std::vector<T> const& vertices, GLuint startAttrib)
        : Bindable(gfx)
        , id_(0)
        , count_(vertices.size())
        , startAttrib_(startAttrib)
        , numAttrs_(0)
    {
        BufferDesc desc;
        BufferData data;

        desc.target = GL_ARRAY_BUFFER;
        desc.usage = GL_DYNAMIC_DRAW;
        desc.byteWidth = sizeof(T) * vertices.size();
        desc.stride = sizeof(T);
        data.mem = vertices.data();

        SetAttribMemLayout<T>();

        gfx_->CreateBuffer(id_, desc, data);
    }

    template <>
    void VertexBuffer::SetAttribMemLayout<VertexPN>();
    template <>
    void VertexBuffer::SetAttribMemLayout<VertexPNT>();
    template <>
    void VertexBuffer::SetAttribMemLayout<VertexPNC>();
    template <>
    void VertexBuffer::SetAttribMemLayout<glm::vec3>();
    template <>
    void VertexBuffer::SetAttribMemLayout<glm::vec2>();

    template <typename T>
    void VertexBuffer::Update(std::vector<T> const& vertices)
    {
        gfx_->UpdateBuffer(id_, GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(T), vertices.data());
    }
}

#endif
