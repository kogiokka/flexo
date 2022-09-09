#include "bindable/program/FragmentShaderProgram.hpp"
#include "bindable/program/ProgramPipeline.hpp"

namespace Bind
{
    FragmentShaderProgram::FragmentShaderProgram(Graphics& gfx, std::string const& filename,
                                                 ProgramPipeline const& pipeline)
        : Bindable(gfx)
        , m_pipeline(pipeline)
    {
        m_gfx->CreateSeparableShaderProgram(m_id, GLWRShaderStage::GLWRShaderStage_Frag, filename);
    }

    FragmentShaderProgram::~FragmentShaderProgram()
    {
        m_gfx->DeleteShaderProgram(m_id);
    }

    void FragmentShaderProgram::Bind()
    {
        m_gfx->SetProgramPipelineStages(m_pipeline.m_id, GL_FRAGMENT_SHADER_BIT, m_id);
    }
}
