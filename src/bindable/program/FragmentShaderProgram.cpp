#include "bindable/program/FragmentShaderProgram.hpp"

namespace Bind
{
    FragmentShaderProgram::FragmentShaderProgram(Graphics& gfx, std::string const& filename)
        : Bindable(gfx)
    {
        std::string const source = Graphics::SlurpShaderSource(filename);
        m_gfx->CreateFragmentShader(source.data(), &m_program);
    }

    FragmentShaderProgram::~FragmentShaderProgram()
    {
    }

    void FragmentShaderProgram::Bind()
    {
        m_gfx->SetFragmentShader(m_program.Get());
    }
}
