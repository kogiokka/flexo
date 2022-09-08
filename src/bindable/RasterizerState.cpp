#include "bindable/RasterizerState.hpp"

namespace Bind
{
    RasterizerState::RasterizerState(Graphics& gfx, RasterizerDesc const& desc)
        : Bindable(gfx)
    {
        ::RasterizerState* ptr = nullptr;
        m_gfx->CreateRasterizerState(desc, &ptr);
        m_state.reset(ptr);
    }

    RasterizerState::~RasterizerState()
    {
    }

    void RasterizerState::Bind()
    {
        m_gfx->SetRasterizerState(m_state.get());
    }
}
