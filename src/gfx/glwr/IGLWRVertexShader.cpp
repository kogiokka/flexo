#include "gfx/glwr/IGLWRVertexShader.hpp"

IGLWRVertexShader::IGLWRVertexShader()
{
    m_id = glCreateProgram();
    glProgramParameteri(m_id, GL_PROGRAM_SEPARABLE, GL_TRUE);
}

IGLWRVertexShader::~IGLWRVertexShader()
{
    glDeleteProgram(m_id);
}
