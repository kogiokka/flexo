#include "bindable/Shader.hpp"

namespace Bind
{
    Shader::Shader(Graphics& gfx)
        : Bindable(gfx)
    {
        m_gfx->CreateShaderProgram(m_id);
    }

    Shader::~Shader()
    {
        m_gfx->DeleteShaderProgram(m_id);
    }

    void Shader::Bind()
    {
        m_gfx->SetShaderProgram(m_id);
    }

    void Shader::Link()
    {
        m_gfx->LinkShaderProgram(m_id);
    }

    void Shader::Attach(ShaderStage stage, std::string const& filepath)
    {
        m_gfx->AttachShaderStage(m_id, stage, filepath);
    }
}
