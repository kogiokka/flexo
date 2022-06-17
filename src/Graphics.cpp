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

void Graphics::SetPrimitive(GLenum primitive)
{
    ctx_.primitive = primitive;
}

void Graphics::SetVertexBuffers(int startAttrib, int numBuffers, GLuint* buffers, GLintptr* offsets, GLsizei* strides)
{
    for (int i = 0; i < numBuffers; i++) {
        int const attr = startAttrib + i;
        glBindBuffer(GL_ARRAY_BUFFER, attr);
        glBindVertexBuffer(attr, buffers[i], offsets[i], strides[i]);
    }
}

void Graphics::SetIndexBuffer(GLuint buffer, GLenum format, const GLvoid* offset)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    ctx_.format = format;
    ctx_.offset = offset;
}

void Graphics::Draw(GLsizei vertexCount)
{
    glDrawArrays(ctx_.primitive, 0, vertexCount);
}

void Graphics::DrawIndexed(GLsizei indexCount)
{
    glDrawElements(ctx_.primitive, indexCount, ctx_.count, ctx_.offset);
}

void Graphics::DrawInstanced(GLsizei vertexCountPerInstance, GLsizei instanceCount)
{
    glDrawArraysInstanced(ctx_.primitive, 0, vertexCountPerInstance, instanceCount);
}
