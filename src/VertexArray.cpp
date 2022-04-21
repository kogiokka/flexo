#include "VertexArray.hpp"

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &id_);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &id_);
}

void VertexArray::AddAttribFormat(VertexAttrib attribIndex, AttribFormat const& format)
{
    glVertexAttribFormat(attribIndex, format.count, format.type, format.normalized, 0);
}
