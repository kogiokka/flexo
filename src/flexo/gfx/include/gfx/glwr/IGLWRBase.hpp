#ifndef I_GLWR_BASE_H
#define I_GLWR_BASE_H

#include <glad/glad.h>

class Graphics;

class IGLWRBase
{
    friend Graphics;

public:
    void Release()
    {
        delete this;
    };

protected:
    virtual ~IGLWRBase() {};

    GLuint m_id;
};

#endif
