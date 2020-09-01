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
  GLuint id_;
  std::unordered_map<std::string, GLuint> attribTable_;

public:
  VertexArray();
  ~VertexArray();
  GLuint Id();
  void Bind() const;
  bool AddAttribFormat(std::string const& attribName, GLuint index, AttribFormat const& format);
  void Enable(std::string const& attribName);
  void Disable(std::string const& attribName);
};
