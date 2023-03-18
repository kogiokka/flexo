#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <cstring>
#include <vector>

#include "gfx/Graphics.hpp"
#include "gfx/Vertex.hpp"
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
        VertexBuffer(Graphics& gfx, std::vector<Vertex> const& vertices, unsigned int startAttrib = 0);
        ~VertexBuffer();
        void Bind(Graphics& gfx) override;
        void Update(Graphics& gfx, std::vector<Vertex> const& vertices);
        unsigned int GetStartAttrib() const;
        unsigned int GetCount() const;
    private:
        std::vector<unsigned char> GenBuffer(std::vector<Vertex> const& vertices) const;
    };
}

#endif
