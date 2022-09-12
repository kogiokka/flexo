#include "gfx/glwr/IGLWRFragmentShader.hpp"

IGLWRFragmentShader::IGLWRFragmentShader()
{
    m_id = glCreateProgram();
    glProgramParameteri(m_id, GL_PROGRAM_SEPARABLE, GL_TRUE);
}

IGLWRFragmentShader::~IGLWRFragmentShader()
{
    glDeleteProgram(m_id);
}
