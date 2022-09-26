#ifndef I_GLWR_DEPTH_STENCIL_VIEW_H
#define I_GLWR_DEPTH_STENCIL_VIEW_H

#include "gfx/glwr/IGLWRBase.hpp"
#include "gfx/glwr/IGLWRResource.hpp"

typedef enum {
    GLWRDepthStencilViewType_RenderBuffer,
    GLWRDepthStencilViewType_RenderBuffer_MS,
    GLWRDepthStencilViewType_Texture1D,
    GLWRDepthStencilViewType_Texture2D,
    GLWRDepthStencilViewType_Texture2D_MS,
    GLWRDepthStencilViewType_Texture3D,
} GLWRDepthStencilViewType;

class Graphics;

class IGLWRDepthStencilView : public IGLWRBase
{
    friend Graphics;

private:
    IGLWRDepthStencilView();
    ~IGLWRDepthStencilView() override;

    GLuint m_sampler;
    GLuint m_clearFrame;
    GLWRDepthStencilViewType m_type;
};

#endif
