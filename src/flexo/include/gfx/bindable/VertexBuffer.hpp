#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <cstring>
#include <vector>

#include "gfx/Graphics.hpp"
#include "gfx/VertexArray.hpp"
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
        VertexBuffer(Graphics& gfx, VertexArray const& vertices, unsigned int startAttrib = 0);
        ~VertexBuffer();
        void Bind(Graphics& gfx) override;
        void Update(Graphics& gfx, VertexArray const& vertices);
        unsigned int GetStartAttrib() const;
        unsigned int GetCount() const;
    };
}

#endif
