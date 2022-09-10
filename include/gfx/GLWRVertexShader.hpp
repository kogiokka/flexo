#ifndef GLWR_VERTEX_SHADER_H
#define GLWR_VERTEX_SHADER_H

#include <glad/glad.h>

class Graphics;
class GLWRVertexShader
{
    friend Graphics;
    GLuint m_id;

public:
    GLWRVertexShader();
    ~GLWRVertexShader();
};

#endif
