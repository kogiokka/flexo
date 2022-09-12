#include "gfx/glwr/IGLWRInputLayout.hpp"

IGLWRInputLayout::IGLWRInputLayout()
{
    glGenVertexArrays(1, &m_id);
}

IGLWRInputLayout::~IGLWRInputLayout()
{
    glDeleteVertexArrays(1, &m_id);
}
