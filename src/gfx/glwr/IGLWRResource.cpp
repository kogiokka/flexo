#include "gfx/glwr/IGLWRResource.hpp"

IGLWRResource::IGLWRResource()
{
    glGenTextures(1, &m_id);
}

IGLWRResource::~IGLWRResource()
{
    glDeleteTextures(1, &m_id);
}
