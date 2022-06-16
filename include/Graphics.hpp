#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <glad/glad.h>

struct BufferDesc {
    GLenum target;
    GLenum usage;
    GLsizeiptr byteWidth;
};

struct BufferData {
    const void* mem;
};

class Graphics
{

public:
    void ClearBuffer(float red, float green, float blue) const;
    void CreateBuffer(GLuint& buffer, BufferDesc const& desc, BufferData const& data);
    void SetVertexBuffer(GLuint buffer);
    void SetIndexBuffer(GLuint buffer, GLenum format, const GLvoid* offset);
};

#endif
