#include "gfx/GLWRFragmentShader.hpp"

GLWRFragmentShader::GLWRFragmentShader()
{
    m_id = glCreateProgram();
    glProgramParameteri(m_id, GL_PROGRAM_SEPARABLE, GL_TRUE);
}

GLWRFragmentShader::~GLWRFragmentShader()
{
    glDeleteProgram(m_id);
}
