#include "gfx/glwr/IGLWRRenderTargetView.hpp"
#include "common/Logger.hpp"

IGLWRRenderTargetView::IGLWRRenderTargetView()
{
    glGenFramebuffers(1, &m_id);
    glGenTextures(1, &m_textureView);
    glGenSamplers(1, &m_sampler);

    glSamplerParameteri(m_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(m_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

IGLWRRenderTargetView::~IGLWRRenderTargetView()
{
    // glDeleteRenderbuffers(1, &m_rbo);
    glDeleteTextures(1, &m_textureView);
    glDeleteSamplers(1, &m_sampler);
}
