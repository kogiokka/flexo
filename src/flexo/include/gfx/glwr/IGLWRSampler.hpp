#ifndef I_GLWR_SAMPLER_H
#define I_GLWR_SAMPLER_H

#include "gfx/glwr/IGLWRBase.hpp"

class Graphics;
class IGLWRRenderTargetView;

class IGLWRSampler : public IGLWRBase
{
    friend Graphics;
    friend IGLWRRenderTargetView;

private:
    IGLWRSampler();
    ~IGLWRSampler() override;
};

#endif
