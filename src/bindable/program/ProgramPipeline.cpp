#include "bindable/program/ProgramPipeline.hpp"

namespace Bind
{
    ProgramPipeline::ProgramPipeline(Graphics& gfx)
        : Bindable(gfx)
    {
        m_gfx->CreateProgramPipeline(m_id);
    }

    ProgramPipeline::~ProgramPipeline() { m_gfx->DeleteProgramPipeline(m_id); }

    void ProgramPipeline::Bind() { m_gfx->SetProgramPipeline(m_id); }

    GLuint ProgramPipeline::GetId() const { return m_id; }
}
