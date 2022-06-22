#include "glm/gtc/type_ptr.hpp"

#include "bindable/Shader.hpp"

Shader::Shader(Graphics& gfx)
{
    gfx.CreateShaderProgram(id_);
}

void Shader::Bind(Graphics& gfx)
{
    gfx.SetShaderProgram(id_);
}

void Shader::Link(Graphics& gfx)
{
    gfx.LinkShaderProgram(id_);
}

void Shader::Attach(Graphics& gfx, ShaderStage stage, std::string const& filepath)
{
    gfx.AttachShaderStage(id_, stage, filepath);
}
