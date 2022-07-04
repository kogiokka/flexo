#include "bindable/program/VertexShaderProgram.hpp"

namespace Bind
{
    VertexShaderProgram::VertexShaderProgram(Graphics& gfx, std::string const& filename, ProgramPipeline* pipeline)
        : Bindable(gfx)
        , m_pipeline(pipeline)
    {
        m_gfx->CreateSeparableShaderProgram(m_id, ShaderStage::Vert, filename);
    }

    VertexShaderProgram::~VertexShaderProgram() { m_gfx->DeleteShaderProgram(m_id); }

    void VertexShaderProgram::Bind()
    {
        m_gfx->SetProgramPipelineStages(m_pipeline->GetId(), GL_VERTEX_SHADER_BIT, m_id);
    }
}
