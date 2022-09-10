#ifndef GLWR_TEXTURE_2D_H
#define GLWR_TEXTURE_2D_H

#include <glad/glad.h>

class Graphics;
class GLWRRenderTarget;

class GLWRTexture2D
{
    friend Graphics;
    friend GLWRRenderTarget;
    GLuint m_id;

public:
    GLWRTexture2D();
    ~GLWRTexture2D();
};

#endif
