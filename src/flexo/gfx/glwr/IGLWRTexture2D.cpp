#include "gfx/glwr/IGLWRTexture2D.hpp"

IGLWRTexture2D::IGLWRTexture2D()
    : IGLWRResource()
{
    glGenTextures(1, &m_id);
}

IGLWRTexture2D::~IGLWRTexture2D()
{
    glDeleteTextures(1, &m_id);
}
