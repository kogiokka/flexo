#ifndef I_GLWR_INPUT_LAYOUT_H
#define I_GLWR_INPUT_LAYOUT_H

#include "gfx/glwr/IGLWRBase.hpp"

class Graphics;

class IGLWRInputLayout : public IGLWRBase
{
    friend Graphics;

private:
    IGLWRInputLayout();
    ~IGLWRInputLayout() override;

    GLuint m_id;
};

#endif
