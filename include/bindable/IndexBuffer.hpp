#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#include <vector>

#include <glad/glad.h>

#include "bindable/Bindable.hpp"
#include "gfx/Graphics.hpp"

namespace Bind
{
    class IndexBuffer : public Bindable
    {
    protected:
        GLWRPtr<GLWRBuffer> m_buffer;
        GLuint m_count;

    public:
        IndexBuffer(Graphics& gfx, std::vector<unsigned int> const& indices);
        ~IndexBuffer() override;
        void Bind() override;
        GLuint GetCount() const;
    };
}

#endif
