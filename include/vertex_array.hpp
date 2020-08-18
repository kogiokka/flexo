#pragma once

#include "glad/glad.h"

#include <string>
#include <unordered_map>

struct AttribFormat {
  GLint count;
  GLenum type;
  GLboolean normalized;
};

class VertexArray
{
  GLuint m_id;
  std::unordered_map<std::string, GLuint> m_indexTable;

public:
  VertexArray();
  ~VertexArray();
  GLuint id();
  void bind() const;
  bool addAttrib(std::string const& attribName, GLuint index, AttribFormat const& format);
  void enable(std::string const& attribName);
  void disable(std::string const& attribName);
};
