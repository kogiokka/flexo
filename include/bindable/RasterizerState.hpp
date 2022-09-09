#ifndef RASTERIZER_STATE_H
#define RASTERIZER_STATE_H

#include <memory>

#include <glad/glad.h>

#include "bindable/Bindable.hpp"
#include "gfx/Graphics.hpp"

namespace Bind
{
    class RasterizerState : public Bindable
    {
    protected:
        GLWRPtr<GLWRRasterizerState> m_state;

    public:
        RasterizerState(Graphics& gfx, GLWRRasterizerDesc const& desc);
        ~RasterizerState() override;
        void Bind() override;
    };
}

#endif
