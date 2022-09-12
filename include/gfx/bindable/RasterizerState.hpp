#ifndef RASTERIZER_STATE_H
#define RASTERIZER_STATE_H

#include <memory>

#include <glad/glad.h>

#include "gfx/Graphics.hpp"
#include "gfx/bindable/Bindable.hpp"

namespace Bind
{
    class RasterizerState : public Bindable
    {
    protected:
        GLWRPtr<IGLWRRasterizerState> m_state;

    public:
        RasterizerState(Graphics& gfx, GLWRRasterizerDesc const& desc);
        ~RasterizerState() override;
        void Bind() override;
    };
}

#endif
