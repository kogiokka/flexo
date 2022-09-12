#ifndef I_GLWR_RENDER_TARGET_H
#define I_GLWR_RENDER_TARGET_H

#include "gfx/glwr/IGLWRBase.hpp"
#include "gfx/glwr/IGLWRResource.hpp"
#include "gfx/glwr/IGLWRSampler.hpp"

class Graphics;

class IGLWRRenderTargetView : public IGLWRBase
{
    friend Graphics;

private:
    IGLWRRenderTargetView();
    ~IGLWRRenderTargetView() override;

    GLuint m_textureView;
    GLuint m_sampler;
    GLenum m_dimensions;
};

#endif
