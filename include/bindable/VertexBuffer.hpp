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
        GLuint m_startAttrib;
        GLsizei m_stride;
        GLuint m_count;

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
        , m_startAttrib(startAttrib)
        , m_stride(sizeof(T))
        , m_count(vertices.size())
    {
        BufferDesc desc;
        BufferData data;

        desc.target = GL_ARRAY_BUFFER;
        desc.usage = GL_DYNAMIC_DRAW;
        desc.byteWidth = sizeof(T) * vertices.size();
        desc.stride = m_stride;
        data.mem = vertices.data();

        m_gfx->CreateBuffer(m_id, desc, data);
    }

    template <typename T>
    void VertexBuffer::Update(std::vector<T> const& vertices)
    {
        m_gfx->UpdateBuffer(m_id, GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(T), vertices.data());
    }
}

#endif
