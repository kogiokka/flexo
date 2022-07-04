#include "bindable/program/ShaderProgram.hpp"

namespace Bind
{
    ShaderProgram::ShaderProgram(Graphics& gfx)
        : Bindable(gfx)
    {
        m_gfx->CreateShaderProgram(m_id);
    }

    ShaderProgram::~ShaderProgram()
    {
        m_gfx->DeleteShaderProgram(m_id);
    }

    void ShaderProgram::Bind()
    {
        m_gfx->SetShaderProgram(m_id);
    }

    void ShaderProgram::Link()
    {
        m_gfx->LinkShaderProgram(m_id);
    }

    void ShaderProgram::Attach(ShaderStage stage, std::string const& filepath)
    {
        m_gfx->AttachShaderStage(m_id, stage, filepath);
    }
}
