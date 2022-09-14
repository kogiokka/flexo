#ifndef I_GLWR_RESOURCE_H
#define I_GLWR_RESOURCE_H

#include "gfx/glwr/IGLWRBase.hpp"

typedef enum {
    GLWRResourceType_Buffer,
    GLWRResourceType_Texture1D,
    GLWRResourceType_Texture2D,
    GLWRResourceType_Texture3D,
    GLWRResourceType_RenderBuffer,
} GLWRResourceType;

class Graphics;

class IGLWRResource : public IGLWRBase
{
    friend Graphics;

protected:
    IGLWRResource();
    virtual ~IGLWRResource() override;

    GLWRResourceType m_type;
};

#endif
