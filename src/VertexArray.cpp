#include "VertexArray.hpp"

VertexArray::VertexArray()
{
  glCreateVertexArrays(1, &id_);
}

VertexArray::~VertexArray()
{
  glDeleteVertexArrays(1, &id_);
}

bool
VertexArray::AddAttribFormat(std::string const& attribName, GLuint index, AttribFormat const& format)
{
  if (attribTable_.find(attribName) != attribTable_.end()) {
    return false;
  }
  attribTable_.emplace(attribName, index);
  glVertexArrayAttribFormat(id_, index, format.count, format.type, format.normalized, 0);
  return true;
}
