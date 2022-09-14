#ifndef I_GLWR_DEPTH_STENCIL_VIEW_H
#define I_GLWR_DEPTH_STENCIL_VIEW_H

#include "gfx/glwr/IGLWRBase.hpp"
#include "gfx/glwr/IGLWRResource.hpp"

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
    GLWRResourceType m_resourceType;
};

#endif
