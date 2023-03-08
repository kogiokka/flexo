#include "gfx/glwr/IGLWRSampler.hpp"

IGLWRSampler::IGLWRSampler()
{
    glGenSamplers(1, &m_id);
}

IGLWRSampler::~IGLWRSampler()
{
    glDeleteSamplers(1, &m_id);
}
