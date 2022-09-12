#include "gfx/bindable/program/VertexShaderProgram.hpp"

namespace Bind
{
    VertexShaderProgram::VertexShaderProgram(Graphics& gfx, std::string const& filename)
        : Bindable(gfx)
    {

        std::string source = Graphics::SlurpShaderSource(filename);
        m_gfx->CreateVertexShader(source.data(), &m_program);
    }

    VertexShaderProgram::~VertexShaderProgram()
    {
    }

    void VertexShaderProgram::Bind()
    {
        m_gfx->SetVertexShader(m_program.Get());
    }
}
