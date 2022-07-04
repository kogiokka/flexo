#include "bindable/program/FragmentShaderProgram.hpp"

namespace Bind
{
    FragmentShaderProgram::FragmentShaderProgram(Graphics& gfx, std::string const& filename, GLuint pipeline)
        : Bindable(gfx)
        , m_pipeline(pipeline)
    {
        m_gfx->CreateSeparableShaderProgram(m_id, ShaderStage::Frag, filename);
    }

    FragmentShaderProgram::~FragmentShaderProgram()
    {
        m_gfx->DeleteShaderProgram(m_id);
    }

    void FragmentShaderProgram::Bind()
    {
        m_gfx->SetProgramPipelineStages(m_pipeline, GL_FRAGMENT_SHADER_BIT, m_id);
    }
}
