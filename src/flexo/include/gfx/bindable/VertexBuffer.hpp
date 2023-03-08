#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <cstring>
#include <vector>

#include "gfx/Graphics.hpp"
#include "gfx/VertexBuffer.hpp"
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
        VertexBuffer(Graphics& gfx, ::VertexBuffer const& buffer, unsigned int startAttrib = 0);
        ~VertexBuffer();
        void Bind(Graphics& gfx) override;
        void Update(Graphics& gfx, ::VertexBuffer const& buffer);
        unsigned int GetStartAttrib() const;
        unsigned int GetCount() const;
    };
}

#endif
