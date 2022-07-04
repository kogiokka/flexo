#include "bindable/program/FragmentShaderProgram.hpp"

namespace Bind
{
    FragmentShaderProgram::FragmentShaderProgram(Graphics& gfx, std::string const& filename, ProgramPipeline* pipeline)
        : Bindable(gfx)
        , m_pipeline(pipeline)
    {
        m_gfx->CreateSeparableShaderProgram(m_id, ShaderStage::Frag, filename);
    }

    FragmentShaderProgram::~FragmentShaderProgram() { m_gfx->DeleteShaderProgram(m_id); }

    void FragmentShaderProgram::Bind()
    {
        m_gfx->SetProgramPipelineStages(m_pipeline->GetId(), GL_FRAGMENT_SHADER_BIT, m_id);
    }
}
