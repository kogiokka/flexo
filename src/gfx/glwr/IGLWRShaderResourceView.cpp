#include "gfx/glwr/IGLWRShaderResourceView.hpp"

IGLWRShaderResourceView::IGLWRShaderResourceView()
{
    glGenTextures(1, &m_id);
}

IGLWRShaderResourceView::~IGLWRShaderResourceView()
{
    glDeleteTextures(1, &m_id);
}
