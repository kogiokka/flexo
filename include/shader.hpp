#pragma once

#include <array>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_map>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader
{
  GLuint m_id;
  std::unordered_map<std::string, GLint> m_uniformLocations;

public:
  Shader();
  ~Shader();
  Shader(Shader const&) = delete;
  Shader& operator=(Shader const&) = delete;
  Shader(Shader&&) = default;

  unsigned int Id() const;
  bool Attach(GLenum shader_type, std::string_view const filepath);
  void Use() const;
  bool Link() const;
  void SetUniform1i(std::string_view const name, int value);
  void SetUniform1f(std::string_view const name, float value);
  void SetUniform3f(std::string_view const name, float x, float y, float z);
  void SetUniformMatrix4fv(std::string_view const name, glm::mat4 const& mat);
  void SetUniform3fv(std::string_view const name, glm::vec3 const& vec);
  void SetUniform3fv(std::string_view const name, std::array<float, 3> const& vec);

private:
  bool IsCompiled(GLuint const shader_object);
  int UniformLocation(std::string_view const uniform_name);
};
