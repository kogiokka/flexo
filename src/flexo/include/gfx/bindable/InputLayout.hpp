#ifndef VERTEX_LAYOUT_H
#define VERTEX_LAYOUT_H

#include <vector>

#include "gfx/Graphics.hpp"
#include "gfx/bindable/Bindable.hpp"

namespace Bind
{
    class VertexShaderProgram;
    class InputLayout : public Bindable
    {
        GLWRPtr<IGLWRInputLayout> m_inputLayout;

    public:
        InputLayout(Graphics& gfx, std::vector<GLWRInputElementDesc> const& inputElementDesc,
                    VertexShaderProgram* programWithInputSignature);
        ~InputLayout() override;
        void Bind(Graphics& gfx) override;
    };
}

#endif
