#ifndef I_GLWR_RENDER_TARGET_H
#define I_GLWR_RENDER_TARGET_H

#include "gfx/glwr/IGLWRBase.hpp"
#include "gfx/glwr/IGLWRResource.hpp"

typedef enum {
    GLWRRenderTargetViewType_RenderBuffer,
    GLWRRenderTargetViewType_Texture1D,
    GLWRRenderTargetViewType_Texture2D,
    GLWRRenderTargetViewType_Texture3D,
} GLWRRenderTargetViewType;

class Graphics;

class IGLWRRenderTargetView : public IGLWRBase
{
    friend Graphics;

private:
    IGLWRRenderTargetView();
    ~IGLWRRenderTargetView() override;

    GLuint m_sampler;
    GLuint m_clearFrame;
    GLWRRenderTargetViewType m_type;
};

#endif
