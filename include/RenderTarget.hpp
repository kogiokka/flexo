#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include <glad/glad.h>

class RenderTarget
{
    GLuint m_frame;
    GLuint m_texture;
    GLuint m_rbo;

public:
    RenderTarget(int width, int height);
    ~RenderTarget();
    GLuint GetFrame() const;
    GLuint GetTexture() const;
};

#endif
