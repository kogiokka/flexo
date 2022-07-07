#include "bindable/InputLayout.hpp"

namespace Bind
{
    InputLayout::InputLayout(Graphics& gfx, std::vector<InputElementDesc> const& inputElementDesc,
                             VertexShaderProgram const* programWithInputSignature)
        : Bindable(gfx)
    {
        m_gfx->CreateInputLayout(m_id, inputElementDesc.data(), inputElementDesc.size(), programWithInputSignature->GetId());
    }

    InputLayout::~InputLayout() { m_gfx->DeleteInputLayout(m_id); }

    void InputLayout::Bind() { m_gfx->SetInputLayout(m_id); }
}
