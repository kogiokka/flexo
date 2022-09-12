#ifndef I_GLWR_SHADER_RESOURCE_VIEW_H
#define I_GLWR_SHADER_RESOURCE_VIEW_H

#include "gfx/glwr/IGLWRBase.hpp"

class Graphics;

class IGLWRShaderResourceView : public IGLWRBase
{
    friend Graphics;

private:
    IGLWRShaderResourceView();
    ~IGLWRShaderResourceView() override;

    GLenum m_target;
};

#endif
