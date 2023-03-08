#include "gfx/glwr/IGLWRRenderBuffer.hpp"

IGLWRRenderBuffer::IGLWRRenderBuffer()
{
    glGenRenderbuffers(1, &m_id);
}

IGLWRRenderBuffer::~IGLWRRenderBuffer()
{
    glDeleteRenderbuffers(1, &m_id);
}
