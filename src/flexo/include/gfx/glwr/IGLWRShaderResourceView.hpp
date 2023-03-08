#ifndef I_GLWR_SHADER_RESOURCE_VIEW_H
#define I_GLWR_SHADER_RESOURCE_VIEW_H

#include "gfx/glwr/IGLWRBase.hpp"

typedef enum {
    GLWRShaderResourceViewType_Buffer,
    GLWRShaderResourceViewType_Texture1D = GL_TEXTURE_1D,
    GLWRShaderResourceViewType_Texture2D = GL_TEXTURE_2D,
    GLWRShaderResourceViewType_Texture3D = GL_TEXTURE_3D,
} GLWRShaderResourceViewType;

class Graphics;

class IGLWRShaderResourceView : public IGLWRBase
{
    friend Graphics;

private:
    IGLWRShaderResourceView();
    ~IGLWRShaderResourceView() override;

    GLWRShaderResourceViewType m_type;
};

#endif
