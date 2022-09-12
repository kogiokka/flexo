#include "gfx/glwr/GLWRVertexShader.hpp"

GLWRVertexShader::GLWRVertexShader()
{
    m_id = glCreateProgram();
    glProgramParameteri(m_id, GL_PROGRAM_SEPARABLE, GL_TRUE);
}

GLWRVertexShader::~GLWRVertexShader()
{
    glDeleteProgram(m_id);
}
