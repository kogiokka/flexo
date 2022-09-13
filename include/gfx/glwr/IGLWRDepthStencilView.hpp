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

    GLenum m_dimensions;
    GLuint m_sampler;
    GLuint m_clearFrame;
};

#endif
