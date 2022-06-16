#include "Graphics.hpp"

void Graphics::ClearBuffer(float red, float green, float blue) const
{
    glClearColor(red, green, blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphics::CreateBuffer(GLuint& buffer, BufferDesc const& desc, BufferData const& data)
{
    glGenBuffers(1, &buffer);
    glBindBuffer(desc.target, buffer);
    glBufferData(desc.target, desc.byteWidth, data.mem, desc.usage);
}

void Graphics::SetVertexBuffer(GLuint buffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

void Graphics::SetIndexBuffer(GLuint buffer, GLenum format, const GLvoid* offset)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
}
