#include "gfx/glwr/IGLWRBuffer.hpp"

IGLWRBuffer::IGLWRBuffer()
{
    glGenBuffers(1, &m_id);
}

IGLWRBuffer::~IGLWRBuffer()
{
    glDeleteBuffers(1, &m_id);
}
