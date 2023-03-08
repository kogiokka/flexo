#include "gfx/glwr/IGLWRRenderTargetView.hpp"
#include "gfx/glwr/IGLWRSampler.hpp"

IGLWRRenderTargetView::IGLWRRenderTargetView()
{
    glGenTextures(1, &m_id);
    glGenSamplers(1, &m_sampler);
    glGenFramebuffers(1, &m_clearFrame);

    glSamplerParameteri(m_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(m_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

IGLWRRenderTargetView::~IGLWRRenderTargetView()
{
    glDeleteTextures(1, &m_id);
    glDeleteSamplers(1, &m_sampler);
    glDeleteFramebuffers(1, &m_clearFrame);
}
