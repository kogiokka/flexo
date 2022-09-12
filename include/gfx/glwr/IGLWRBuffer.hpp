#ifndef I_GLWR_BUFFER_H
#define I_GLWR_BUFFER_H

#include "gfx/glwr/IGLWRBase.hpp"

class Graphics;

class IGLWRBuffer : public IGLWRBase
{
    friend Graphics;

private:
    IGLWRBuffer();
    ~IGLWRBuffer() override;

    GLuint m_id;
};

#endif
