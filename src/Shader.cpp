#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"

Shader::Shader()
  : id_(glCreateProgram())
{
  assert(id_ != 0);
}

Shader::~Shader()
{
  glDeleteProgram(id_);
}

unsigned int
Shader::Id() const
{
  return static_cast<unsigned int>(id_);
}

bool
Shader::Attach(GLenum shaderType, std::string_view const filepath)
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

  char const* const shaderSourceArray[1] = {source.c_str()};

  GLuint shaderObject = glCreateShader(shaderType);
  glShaderSource(shaderObject, 1, shaderSourceArray, nullptr);
  glCompileShader(shaderObject);
  if (!IsCompiled(shaderObject))
    return false;
  glAttachShader(id_, shaderObject);
  glDeleteShader(shaderObject);

  return true;
}

int
Shader::UniformLocation(std::string_view const uniformName)
{
  char const* name = uniformName.data();

  if (uniformLocations_.find(name) != uniformLocations_.end()) {
    return uniformLocations_[name];
  }

  int location = glGetUniformLocation(id_, name);
  if (location == -1) {
    std::fprintf(stderr, "Uniform %s does not exist.\n", name);
  }
  uniformLocations_[name] = location;
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
}

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
Shader::SetUniform1f(std::string_view const name, float value)
{
  glUniform1f(UniformLocation(name), value);
}

void
Shader::SetUniform1i(std::string_view const name, int value)
{
  glUniform1i(UniformLocation(name), value);
}

void
Shader::Use() const
{
  glUseProgram(id_);
}

bool
Shader::Link() const
{
  glLinkProgram(id_);

  GLint success;
  glGetProgramiv(id_, GL_LINK_STATUS, &success);

  if (success == GL_TRUE)
    return true;

  GLint lenLog;
  std::string log;
  glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &lenLog);
  log.resize(lenLog);
  glGetProgramInfoLog(id_, lenLog, nullptr, log.data());
  glDeleteProgram(id_);

  fprintf(stderr, "%s\n", log.data());
  return false;
}

bool
Shader::IsCompiled(GLuint const shaderObject)
{
  GLint success;
  glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);

  if (success == GL_TRUE)
    return true;

  GLint lenLog;
  std::string log;

  glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &lenLog);
  log.resize(lenLog);
  glGetShaderInfoLog(shaderObject, lenLog, nullptr, log.data());
  glDeleteShader(shaderObject);

  std::fprintf(stderr, "%s\n", log.data());

  return false;
}
