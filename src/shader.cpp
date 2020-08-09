#include "shader.hpp"
#include <glm/gtc/type_ptr.hpp>

Shader::Shader()
  : m_id(glCreateProgram())
{
  assert(m_id != 0);
}

Shader::~Shader()
{
  glDeleteProgram(m_id);
}

unsigned int
Shader::Id() const
{
  return static_cast<unsigned int>(m_id);
}

bool
Shader::Attach(GLenum shader_type, std::string_view const filepath)
{
  using namespace std;

  ifstream file;
  string source;

  file.open(filepath.data());
  if (file.fail())
    return false;
  source.resize(filesystem::file_size(filepath));
  file.read(source.data(), source.size());
  file.close();

  char const* const shader_source_array[1] = {source.c_str()};

  GLuint shader_object = glCreateShader(shader_type);
  glShaderSource(shader_object, 1, shader_source_array, nullptr);
  glCompileShader(shader_object);
  if (!IsCompiled(shader_object))
    return false;
  glAttachShader(m_id, shader_object);
  glDeleteShader(shader_object);

  return true;
}

int
Shader::UniformLocation(std::string_view const uniform_name)
{
  char const* name = uniform_name.data();

  if (m_uniformLocations.find(name) != m_uniformLocations.end()) {
    return m_uniformLocations[name];
  }

  int location = glGetUniformLocation(m_id, name);
  if (location == -1) {
    fprintf(stderr, "Uniform %s does not exist.\n", name);
  }
  m_uniformLocations[name] = location;
  return location;
}

void
Shader::SetUniformMatrix4fv(std::string_view const name, glm::mat4 const& mat)
{
  glUniformMatrix4fv(UniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void
Shader::SetUniform3f(std::string_view const name, float x, float y, float z)
{
  glUniform3f(UniformLocation(name), x, y, z);
};

void
Shader::SetUniform3fv(std::string_view const name, glm::vec3 const& vec)
{
  glUniform3fv(UniformLocation(name), 1, glm::value_ptr(vec));
}

void
Shader::SetUniform3fv(std::string_view const name, std::array<float, 3> const& vec)
{
  glUniform3fv(UniformLocation(name), 1, vec.data());
}

void
Shader::SetUniform1f(const std::string_view name, float value)
{
  glUniform1f(UniformLocation(name), value);
}

void
Shader::SetUniform1i(const std::string_view name, int value)
{
  glUniform1f(UniformLocation(name), value);
}

void
Shader::Use() const
{
  glUseProgram(m_id);
}

bool
Shader::Link() const
{
  glLinkProgram(m_id);

  GLint success;
  glGetProgramiv(m_id, GL_LINK_STATUS, &success);

  if (success == GL_TRUE)
    return true;

  GLint log_length;
  std::string log;
  glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &log_length);
  log.resize(log_length);
  glGetProgramInfoLog(m_id, log_length, nullptr, log.data());
  glDeleteProgram(m_id);

  fprintf(stderr, "%s\n", log.data());
  return false;
}

bool
Shader::IsCompiled(GLuint const shader_object)
{
  GLint success;
  glGetShaderiv(shader_object, GL_COMPILE_STATUS, &success);

  if (success == GL_TRUE)
    return true;

  GLint log_length;
  std::string log;

  glGetShaderiv(shader_object, GL_INFO_LOG_LENGTH, &log_length);
  log.resize(log_length);
  glGetShaderInfoLog(shader_object, log_length, nullptr, log.data());
  glDeleteShader(shader_object);

  fprintf(stderr, "%s\n", log.data());

  return false;
}
