#include "gfx/bindable/RasterizerState.hpp"

namespace Bind
{
    RasterizerState::RasterizerState(Graphics& gfx, GLWRRasterizerDesc const& desc)
    {
        gfx.CreateRasterizerState(&desc, &m_state);
    }

    RasterizerState::~RasterizerState()
    {
    }

    void RasterizerState::Bind(Graphics& gfx)
    {
        gfx.SetRasterizerState(m_state.Get());
    }
}
