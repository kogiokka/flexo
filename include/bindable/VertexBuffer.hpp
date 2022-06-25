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
        GLuint m_id;
        GLuint m_count;
        GLuint m_startAttrib;
        int m_numAttrs;
        std::vector<GLintptr> m_offsets;
        std::vector<GLsizei> m_strides;

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
        , m_id(0)
        , m_count(vertices.size())
        , m_startAttrib(startAttrib)
        , m_numAttrs(0)
    {
        BufferDesc desc;
        BufferData data;

        desc.target = GL_ARRAY_BUFFER;
        desc.usage = GL_DYNAMIC_DRAW;
        desc.byteWidth = sizeof(T) * vertices.size();
        desc.stride = sizeof(T);
        data.mem = vertices.data();

        SetAttribMemLayout<T>();

        m_gfx->CreateBuffer(m_id, desc, data);
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
        m_gfx->UpdateBuffer(m_id, GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(T), vertices.data());
    }
}

#endif
