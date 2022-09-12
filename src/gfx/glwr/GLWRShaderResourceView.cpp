#include "gfx/glwr/GLWRShaderResourceView.hpp"

GLWRShaderResourceView::GLWRShaderResourceView()
{
    glGenTextures(1, &m_id);
}

GLWRShaderResourceView::~GLWRShaderResourceView()
{
    glDeleteTextures(1, &m_id);
}
