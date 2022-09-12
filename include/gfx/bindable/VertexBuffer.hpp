#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <vector>

#include "Vertex.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/bindable/Bindable.hpp"

namespace Bind
{
    class VertexBuffer : public Bindable
    {
    protected:
        GLWRPtr<GLWRBuffer> m_buffer;
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
        , m_startAttrib(startAttrib)
        , m_stride(sizeof(T))
        , m_count(vertices.size())
    {
        GLWRBufferDesc desc;
        GLWRResourceData data;

        desc.target = GL_ARRAY_BUFFER;
        desc.usage = GL_DYNAMIC_DRAW;
        desc.byteWidth = sizeof(T) * vertices.size();
        desc.stride = m_stride;
        data.mem = vertices.data();

        m_gfx->CreateBuffer(&desc, &data, &m_buffer);
    }

    template <typename T>
    void VertexBuffer::Update(std::vector<T> const& vertices)
    {
        GLWRResourceData data;
        data.mem = vertices.data();
        m_gfx->UpdateBuffer(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(T), &data, m_buffer.Get());
    }
}

#endif
