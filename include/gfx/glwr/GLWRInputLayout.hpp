#ifndef GLWR_INPUT_LAYOUT_H
#define GLWR_INPUT_LAYOUT_H

#include <glad/glad.h>

class Graphics;

class GLWRInputLayout
{
    friend Graphics;

    GLuint m_id;

public:
    GLWRInputLayout();
    ~GLWRInputLayout();
};

#endif
