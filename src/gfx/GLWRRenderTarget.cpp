#include "gfx/GLWRRenderTarget.hpp"
#include "common/Logger.hpp"

GLWRRenderTarget::GLWRRenderTarget(int width, int height)
{
    m_texture = new GLWRTexture2D();

    glGenFramebuffers(1, &m_frame);
    glGenRenderbuffers(1, &m_rbo);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_frame);

    // Attach color buffer
    GLint mipmapLevel = 0;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->m_id, mipmapLevel);
    // Attach depth buffer
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Logger::error("Framebuffer failed to generate!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLWRRenderTarget::~GLWRRenderTarget()
{
    glDeleteRenderbuffers(1, &m_rbo);
    glDeleteFramebuffers(1, &m_frame);
    delete m_texture;
}
