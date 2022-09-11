#ifndef GLWR_SHADER_RESOURCE_VIEW_H
#define GLWR_SHADER_RESOURCE_VIEW_H

#include <glad/glad.h>

class Graphics;

class GLWRShaderResourceView
{
    friend Graphics;

    GLuint m_id;
    GLenum m_target;

public:
    GLWRShaderResourceView();
    ~GLWRShaderResourceView();
};

#endif
