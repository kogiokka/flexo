#ifndef I_GLWR_VERTEX_SHADER_H
#define I_GLWR_VERTEX_SHADER_H

#include "gfx/glwr/IGLWRBase.hpp"

class Graphics;

class IGLWRVertexShader : public IGLWRBase
{
    friend Graphics;

private:
    IGLWRVertexShader();
    ~IGLWRVertexShader() override;
};

#endif
