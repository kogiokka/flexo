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

    GLenum m_dimensions;
    GLuint m_sampler;
    GLuint m_clearFrame;
};

#endif
