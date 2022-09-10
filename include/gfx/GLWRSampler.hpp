#ifndef GLWR_SAMPLER_STATE_H
#define GLWR_SAMPLER_STATE_H

#include <glad/glad.h>

class Graphics;
class GLWRRenderTarget;

class GLWRSampler
{
    friend Graphics;
    friend GLWRRenderTarget;

    GLuint m_id;

public:
    GLWRSampler();
    ~GLWRSampler();
};

#endif
