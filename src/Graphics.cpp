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
    ctx_.stride = desc.stride;
}

void Graphics::CreateTexture2D(GLuint& texture, const Texture2dDesc& desc, const BufferData& data)
{
    constexpr GLint BORDER = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, desc.textureFormat, desc.width, desc.height, BORDER, desc.pixelFormat, desc.dataType,
                 data.mem);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Graphics::SetPrimitive(GLenum primitive)
{
    ctx_.primitive = primitive;
}

void Graphics::SetVertexBuffers(int startAttrib, int numBuffers, GLuint* buffers, GLintptr* offsets)
{
    for (int i = 0; i < numBuffers; i++) {
        int const attr = startAttrib + i;
        glBindBuffer(GL_ARRAY_BUFFER, attr);
        glBindVertexBuffer(attr, buffers[i], offsets[i], ctx_.stride);
    }
}

void Graphics::SetIndexBuffer(GLuint buffer, GLenum format, const GLvoid* offset)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    ctx_.format = format;
    ctx_.offset = offset;
}

void Graphics::SetTexture(GLenum target, GLuint texture)
{
    glBindTexture(target, texture);
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
