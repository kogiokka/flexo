#ifndef I_GLWR_TEXTURE_2D_H
#define I_GLWR_TEXTURE_2D_H

#include "gfx/glwr/IGLWRResource.hpp"

class Graphcis;
class IGLWRRenderTargetView;

class IGLWRTexture2D : public IGLWRResource
{
    friend Graphics;
    friend IGLWRRenderTargetView;

private:
    IGLWRTexture2D();
    ~IGLWRTexture2D() override;
};

#endif
