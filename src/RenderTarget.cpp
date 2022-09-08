#include "RenderTarget.hpp"
#include "common/Logger.hpp"

RenderTarget::RenderTarget(int width, int height)
{
    glGenFramebuffers(1, &m_frame);
    glGenTextures(1, &m_texture);
    glGenRenderbuffers(1, &m_rbo);

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_frame);

    // Attach color buffer
    GLint mipmapLevel = 0;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, mipmapLevel);
    // Attach depth buffer
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Logger::error("Framebuffer failed to generate!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget()
{
    glDeleteRenderbuffers(1, &m_rbo);
    glDeleteTextures(1, &m_texture);
    glDeleteFramebuffers(1, &m_frame);
}

GLuint RenderTarget::GetFrame() const
{
    return m_frame;
}

GLuint RenderTarget::GetTexture() const
{
    return m_texture;
}
