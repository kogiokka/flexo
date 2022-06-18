#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <glad/glad.h>

struct BufferDesc {
    GLenum target;
    GLenum usage;
    GLsizeiptr byteWidth;
    GLsizei stride;
};

struct Texture2dDesc {
    GLsizei width;
    GLsizei height;
    GLenum textureFormat;
    GLenum pixelFormat;
    GLenum dataType;
};

struct BufferData {
    const void* mem;
};

class Graphics
{
    struct Context {
        GLenum primitive;
        GLenum format;
        GLsizei count;
        GLsizei stride;
        const GLvoid* offset;
    };

    Context ctx_;

public:
    void ClearBuffer(float red, float green, float blue) const;
    void CreateBuffer(GLuint& buffer, BufferDesc const& desc, BufferData const& data);
    void CreateTexture2D(GLuint& texture, Texture2dDesc const& desc, BufferData const& data);
    void SetPrimitive(GLenum primitive);
    void SetVertexBuffers(int startAttrib, int numBuffers, GLuint* buffers, GLintptr* offsets);
    void SetIndexBuffer(GLuint buffer, GLenum format, const GLvoid* offset);
    void SetTexture(GLenum target, GLuint texture);
    void Draw(GLsizei vertexCount);
    void DrawIndexed(GLsizei indexCount);
    void DrawInstanced(GLsizei vertexCountPerInstance, GLsizei instanceCount);
};

#endif
