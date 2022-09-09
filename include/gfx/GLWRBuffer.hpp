#ifndef GLWR_BUFFER_H
#define GLWR_BUFFER_H

#include <glad/glad.h>

class Graphics;

class GLWRBuffer
{
    friend Graphics;
    GLuint m_id;

public:
    GLWRBuffer();
    ~GLWRBuffer();
};

#endif
