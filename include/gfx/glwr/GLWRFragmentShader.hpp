#ifndef GLWR_FRAGMENT_SHADER_H
#define GLWR_FRAGMENT_SHADER_H

#include <glad/glad.h>

class Graphics;
class GLWRFragmentShader
{
    friend Graphics;
    GLuint m_id;

public:
    GLWRFragmentShader();
    ~GLWRFragmentShader();
};

#endif
