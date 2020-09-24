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
  GLuint Id() const;
  void Bind() const;
  bool AddAttribFormat(std::string const& attribName, GLuint index, AttribFormat const& format);
  void Enable(std::string const& attribName) const;
  void Disable(std::string const& attribName) const;
};

inline void
VertexArray::Bind() const
{
  glBindVertexArray(id_);
}

inline GLuint
VertexArray::Id() const
{
  return id_;
}

inline void
VertexArray::Enable(std::string const& attribName) const
{
  glEnableVertexArrayAttrib(id_, attribTable_.at(attribName));
}

inline void
VertexArray::Disable(std::string const& attribName) const
{
  glDisableVertexArrayAttrib(id_, attribTable_.at(attribName));
}
