#include "gfx/bindable/program/FragmentShaderProgram.hpp"

namespace Bind
{
    FragmentShaderProgram::FragmentShaderProgram(Graphics& gfx, std::string const& filename)
    {
        std::string const source = Graphics::SlurpShaderSource(filename);
        gfx.CreateFragmentShader(source.data(), &m_program);
    }

    FragmentShaderProgram::~FragmentShaderProgram()
    {
    }

    void FragmentShaderProgram::Bind(Graphics& gfx)
    {
        gfx.SetFragmentShader(m_program.Get());
    }
}
