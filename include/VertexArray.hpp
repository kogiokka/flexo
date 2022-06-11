#pragma once

#include "glad/glad.h"

#include <string>

using VertexAttrib = GLuint;

enum VertexAttrib_ {
    VertexAttrib_Position = 0,
    VertexAttrib_Normal = 1,
    VertexAttrib_Translation = 2,
    VertexAttrib_TextureCoord = 3,
};

struct AttribFormat {
    GLint count;
    GLenum type;
    GLboolean normalized;
};

class VertexArray
{
    GLuint id_;

public:
    VertexArray();
    ~VertexArray();
    GLuint Id() const;
    void Bind() const;
    void AddAttribFormat(VertexAttrib attribIndex, AttribFormat const& format);
    void Enable(VertexAttrib attribIndex) const;
    void Disable(VertexAttrib attribIndex) const;
};

inline void VertexArray::Bind() const
{
    glBindVertexArray(id_);
}

inline GLuint VertexArray::Id() const
{
    return id_;
}

inline void VertexArray::Enable(VertexAttrib attribIndex) const
{
    glEnableVertexAttribArray(attribIndex);
}

inline void VertexArray::Disable(VertexAttrib attribIndex) const
{
    glDisableVertexAttribArray(attribIndex);
}
