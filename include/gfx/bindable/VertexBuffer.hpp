#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <cstring>
#include <vector>

#include "Vertex.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/bindable/Bindable.hpp"

namespace Bind
{
    class VertexBuffer : public Bindable
    {
    protected:
        GLWRPtr<IGLWRBuffer> m_buffer;
        unsigned int m_startAttrib;
        unsigned int m_stride;
        unsigned int m_count;

    public:
        template <typename T>
        VertexBuffer(Graphics& gfx, std::vector<T> const& vertices, unsigned int startAttrib = 0);
        ~VertexBuffer();
        void Bind(Graphics& gfx) override;
        template <typename T>
        void Update(Graphics& gfx, std::vector<T> const& vertices);
        unsigned int GetStartAttrib() const;
        unsigned int GetCount() const;

    protected:
        template <typename T>
        void SetAttribMemLayout();
    };

    template <typename T>
    VertexBuffer::VertexBuffer(Graphics& gfx, std::vector<T> const& vertices, unsigned int startAttrib)
        : m_startAttrib(startAttrib)
        , m_stride(sizeof(T))
        , m_count(vertices.size())
    {
        GLWRBufferDesc desc;
        GLWRResourceData data;

        desc.type = GLWRResourceType_Buffer;
        desc.usage = GL_DYNAMIC_DRAW;
        desc.byteWidth = sizeof(T) * vertices.size();
        desc.stride = m_stride;
        data.mem = vertices.data();

        gfx.CreateBuffer(&desc, &data, &m_buffer);
    }

    template <typename T>
    void VertexBuffer::Update(Graphics& gfx, std::vector<T> const& vertices)
    {
        GLWRMappedSubresource mem;
        gfx.Map(m_buffer.Get(), GLWRMapPermission_WriteOnly, &mem);
        std::memcpy(mem.data, vertices.data(), vertices.size() * sizeof(T));
        gfx.Unmap(m_buffer.Get());
    }
}

#endif
