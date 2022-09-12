#ifndef I_GLWR_SAMPLER_H
#define I_GLWR_SAMPLER_H

#include "gfx/glwr/IGLWRBase.hpp"

class Graphics;
class IGLWRRenderTarget;

class IGLWRSampler : public IGLWRBase
{
    friend Graphics;
    friend IGLWRRenderTarget;

private:
    IGLWRSampler();
    ~IGLWRSampler() override;
};

#endif
