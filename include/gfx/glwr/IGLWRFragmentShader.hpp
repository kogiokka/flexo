#ifndef I_GLWR_FRAGMENT_SHADER_H
#define I_GLWR_FRAGMENT_SHADER_H

#include "gfx/glwr/IGLWRBase.hpp"

class Graphics;

class IGLWRFragmentShader : public IGLWRBase
{
    friend Graphics;

private:
    IGLWRFragmentShader();
    ~IGLWRFragmentShader() override;

    GLuint m_id;
};

#endif
