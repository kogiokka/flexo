#include "gfx/GLWRTexture2D.hpp"

GLWRTexture2D::GLWRTexture2D()
{
    glGenTextures(1, &m_id);
}

GLWRTexture2D::~GLWRTexture2D()
{
    glDeleteTextures(1, &m_id);
}
