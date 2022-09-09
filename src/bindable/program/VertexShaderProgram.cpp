#include "bindable/program/VertexShaderProgram.hpp"
#include "bindable/program/ProgramPipeline.hpp"

namespace Bind
{
    VertexShaderProgram::VertexShaderProgram(Graphics& gfx, std::string const& filename,
                                             ProgramPipeline const& pipeline)
        : Bindable(gfx)
        , m_pipeline(pipeline)
    {
        m_gfx->CreateSeparableShaderProgram(m_id, GLWRShaderStage::Vert, filename);
    }

    VertexShaderProgram::~VertexShaderProgram()
    {
        m_gfx->DeleteShaderProgram(m_id);
    }

    void VertexShaderProgram::Bind()
    {
        m_gfx->SetProgramPipelineStages(m_pipeline.m_id, GL_VERTEX_SHADER_BIT, m_id);
    }
}
