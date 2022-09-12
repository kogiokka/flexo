#include "gfx/glwr/GLWRSampler.hpp"

GLWRSampler::GLWRSampler()
{
    glGenSamplers(1, &m_id);
}

GLWRSampler::~GLWRSampler()
{
    glDeleteSamplers(1, &m_id);
}
