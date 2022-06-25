#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#include <vector>

#include <glad/glad.h>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

namespace Bind
{
    class IndexBuffer : public Bindable
    {
    protected:
        GLuint m_id;
        GLuint m_count;

    public:
        IndexBuffer(Graphics& gfx, std::vector<unsigned int> const& indices);
        ~IndexBuffer() override;
        void Bind() override;
        GLuint GetCount() const;
    };
}

#endif
