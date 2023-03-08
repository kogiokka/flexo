#ifndef I_GLWR_RENDER_BUFFER_H
#define I_GLWR_RENDER_BUFFER_H

#include "gfx/glwr/IGLWRResource.hpp"

class Graphics;

class IGLWRRenderBuffer : public IGLWRResource
{
    friend Graphics;

protected:
    IGLWRRenderBuffer();
    virtual ~IGLWRRenderBuffer() override;
};

#endif
