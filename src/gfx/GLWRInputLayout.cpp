#include "gfx/GLWRInputLayout.hpp"

GLWRInputLayout::GLWRInputLayout()
{
    glGenVertexArrays(1, &m_id);
}

GLWRInputLayout::~GLWRInputLayout()
{
    glDeleteVertexArrays(1, &m_id);
}
