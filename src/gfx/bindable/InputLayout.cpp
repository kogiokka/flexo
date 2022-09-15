#include "gfx/bindable/InputLayout.hpp"
#include "gfx/bindable/program/VertexShaderProgram.hpp"

namespace Bind
{
    InputLayout::InputLayout(Graphics& gfx, std::vector<GLWRInputElementDesc> const& inputElementDesc,
                             VertexShaderProgram* programWithInputSignature)
    {
        gfx.CreateInputLayout(inputElementDesc.data(), inputElementDesc.size(),
                              programWithInputSignature->m_program.Get(), &m_inputLayout);
    }

    InputLayout::~InputLayout()
    {
    }

    void InputLayout::Bind(Graphics& gfx)
    {
        gfx.SetInputLayout(m_inputLayout.Get());
    }
}
