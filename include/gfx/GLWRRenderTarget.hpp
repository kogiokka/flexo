#ifndef GLWR_RENDER_TARGET_H
#define GLWR_RENDER_TARGET_H

#include <glad/glad.h>

#include <gfx/GLWRSampler.hpp>
#include <gfx/GLWRTexture2D.hpp>

class Graphics;

class GLWRRenderTarget
{
    friend Graphics;
    GLuint m_frame;
    GLuint m_rbo;
    GLWRSampler* m_sampler;
    GLWRTexture2D* m_texture;

public:
    GLWRRenderTarget(int width, int height);
    ~GLWRRenderTarget();
};

#endif
