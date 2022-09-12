#ifndef GLWR_RESOURCE_H
#define GLWR_RESOURCE_H

#include <glad/glad.h>

class Graphics;

class GLWRResource
{
    friend Graphics;

protected:
    GLuint m_id;

public:
    GLWRResource();
    virtual ~GLWRResource();
};

#endif
