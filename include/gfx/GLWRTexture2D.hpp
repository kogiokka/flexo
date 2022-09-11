#ifndef GLWR_TEXTURE_2D_H
#define GLWR_TEXTURE_2D_H

#include <glad/glad.h>

#include "gfx/GLWRResource.hpp"

class GLWRRenderTarget;

class GLWRTexture2D : public GLWRResource
{
    friend GLWRRenderTarget;

public:
    GLWRTexture2D();
    ~GLWRTexture2D();
};

#endif
