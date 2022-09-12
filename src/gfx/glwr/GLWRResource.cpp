#include "gfx/glwr/GLWRResource.hpp"

GLWRResource::GLWRResource()
{
    glGenTextures(1, &m_id);
}

GLWRResource::~GLWRResource()
{
    glDeleteTextures(1, &m_id);
}
