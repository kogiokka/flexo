#include "gfx/bindable/RasterizerState.hpp"

namespace Bind
{
    RasterizerState::RasterizerState(Graphics& gfx, GLWRRasterizerDesc const& desc)
        : Bindable(gfx)
    {
        m_gfx->CreateRasterizerState(&desc, &m_state);
    }

    RasterizerState::~RasterizerState()
    {
    }

    void RasterizerState::Bind()
    {
        m_gfx->SetRasterizerState(m_state.Get());
    }
}
