#include "glm/gtc/type_ptr.hpp"

#include "bindable/ShaderBindable.hpp"

ShaderBindable::ShaderBindable(Graphics& gfx)
{
    gfx.CreateShaderProgram(id_);
}

void ShaderBindable::Bind(Graphics& gfx)
{
    gfx.SetShaderProgram(id_);
}

void ShaderBindable::Link(Graphics& gfx)
{
    gfx.LinkShaderProgram(id_);
}

void ShaderBindable::Attach(Graphics& gfx, ShaderStage stage, std::string const& filepath)
{
    gfx.AttachShaderStage(id_, stage, filepath);
}
