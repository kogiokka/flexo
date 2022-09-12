#include "gfx/glwr/IGLWRDepthStencilView.hpp"

IGLWRDepthStencilView::IGLWRDepthStencilView()
{
    glGenTextures(1, &m_id);
    glGenSamplers(1, &m_sampler);

    glSamplerParameteri(m_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(m_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

IGLWRDepthStencilView::~IGLWRDepthStencilView()
{
    glDeleteTextures(1, &m_id);
    glDeleteSamplers(1, &m_sampler);
}
