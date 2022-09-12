#ifndef I_GLWR_RENDER_TARGET_H
#define I_GLWR_RENDER_TARGET_H

#include "gfx/glwr/IGLWRSampler.hpp"
#include "gfx/glwr/IGLWRTexture2D.hpp"
#include "gfx/glwr/IGLWRBase.hpp"

class Graphics;

class IGLWRRenderTarget : public IGLWRBase
{
    friend Graphics;

private:
    IGLWRRenderTarget(int width, int height);
    ~IGLWRRenderTarget() override;

    GLuint m_rbo;
    IGLWRSampler* m_sampler;
    IGLWRTexture2D* m_texture;
};

#endif
