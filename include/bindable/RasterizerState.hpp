#ifndef RASTERIZER_STATE_H
#define RASTERIZER_STATE_H

#include <glad/glad.h>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

namespace Bind
{
    class RasterizerState : public Bindable
    {
    protected:
        ::RasterizerState* m_state;

    public:
        RasterizerState(Graphics& gfx, RasterizerDesc const& desc);
        ~RasterizerState() override;
        void Bind() override;
    };
}

#endif
