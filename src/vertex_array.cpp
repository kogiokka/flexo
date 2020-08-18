#include "vertex_array.hpp"

VertexArray::VertexArray()
{
  glCreateVertexArrays(1, &m_id);
}

VertexArray::~VertexArray()
{
  glDeleteVertexArrays(1, &m_id);
}

void
VertexArray::bind() const
{
  glBindVertexArray(m_id);
}

GLuint
VertexArray::id()
{
  return m_id;
}

bool
VertexArray::addAttrib(std::string const& attribName, GLuint index, AttribFormat const& format)
{
  if (m_indexTable.find(attribName) != m_indexTable.end()) {
    return false;
  }
  m_indexTable.insert({attribName, index});
  glVertexArrayAttribFormat(m_id, index, format.count, format.type, format.normalized, 0);
  return true;
}

void
VertexArray::enable(std::string const& attribName)
{
  glEnableVertexArrayAttrib(m_id, m_indexTable[attribName]);
}

void
VertexArray::disable(std::string const& attribName)
{
  glDisableVertexArrayAttrib(m_id, m_indexTable[attribName]);
}
