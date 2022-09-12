#include "gfx/glwr/GLWRBuffer.hpp"

GLWRBuffer::GLWRBuffer()
{
    glGenBuffers(1, &m_id);
}

GLWRBuffer::~GLWRBuffer()
{
    glDeleteBuffers(1, &m_id);
}
