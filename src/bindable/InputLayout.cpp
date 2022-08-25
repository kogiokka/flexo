#include "bindable/InputLayout.hpp"
#include "bindable/program/VertexShaderProgram.hpp"

namespace Bind
{
    InputLayout::InputLayout(Graphics& gfx, std::vector<InputElementDesc> const& inputElementDesc,
                             VertexShaderProgram const* programWithInputSignature)
        : Bindable(gfx)
    {
        m_gfx->CreateInputLayout(m_id, inputElementDesc.data(), inputElementDesc.size(),
                                 programWithInputSignature->m_id);
    }

    InputLayout::~InputLayout()
    {
        m_gfx->DeleteInputLayout(m_id);
    }

    void InputLayout::Bind()
    {
        m_gfx->SetInputLayout(m_id);
    }
}
