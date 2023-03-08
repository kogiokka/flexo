#include "gfx/bindable/program/VertexShaderProgram.hpp"

namespace Bind
{
    VertexShaderProgram::VertexShaderProgram(Graphics& gfx, std::string const& filename)
    {

        std::string source = Graphics::SlurpShaderSource(filename);
        gfx.CreateVertexShader(source.data(), &m_program);
    }

    VertexShaderProgram::~VertexShaderProgram()
    {
    }

    void VertexShaderProgram::Bind(Graphics& gfx)
    {
        gfx.SetVertexShader(m_program.Get());
    }
}
