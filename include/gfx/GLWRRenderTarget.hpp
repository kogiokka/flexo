#ifndef GLWR_RENDER_TARGET_H
#define GLWR_RENDER_TARGET_H

#include <glad/glad.h>

class GLWRRenderTarget
{
    GLuint m_frame;
    GLuint m_texture;
    GLuint m_rbo;

public:
    GLWRRenderTarget(int width, int height);
    ~GLWRRenderTarget();
    GLuint GetFrame() const;
    GLuint GetTexture() const;
};

#endif
