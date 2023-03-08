#ifndef I_GLWR_BUFFER_H
#define I_GLWR_BUFFER_H

#include "gfx/glwr/IGLWRBase.hpp"
#include "gfx/glwr/IGLWRResource.hpp"

class Graphics;

class IGLWRBuffer : public IGLWRResource
{
    friend Graphics;

private:
    IGLWRBuffer();
    ~IGLWRBuffer() override;
};

#endif
