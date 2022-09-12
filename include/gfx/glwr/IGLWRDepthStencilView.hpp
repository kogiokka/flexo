#ifndef I_GLWR_DEPTH_STENCIL_VIEW_H
#define I_GLWR_DEPTH_STENCIL_VIEW_H

#include "gfx/glwr/IGLWRBase.hpp"

class Graphics;

class IGLWRDepthStencilView : public IGLWRBase
{
    friend Graphics;

private:
    IGLWRDepthStencilView();
    ~IGLWRDepthStencilView() override;

    GLuint m_parent; // framebuffer
    GLuint m_sampler;
    GLenum m_dimensions;
};

#endif
