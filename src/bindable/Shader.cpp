#include "bindable/Shader.hpp"

namespace Bind
{
    Shader::Shader(Graphics& gfx)
        : Bindable(gfx)
    {
        gfx_->CreateShaderProgram(id_);
    }

    Shader::~Shader()
    {
        gfx_->DeleteShaderProgram(id_);
    }

    void Shader::Bind()
    {
        gfx_->SetShaderProgram(id_);
    }

    void Shader::Link()
    {
        gfx_->LinkShaderProgram(id_);
    }

    void Shader::Attach(ShaderStage stage, std::string const& filepath)
    {
        gfx_->AttachShaderStage(id_, stage, filepath);
    }
}
