#ifndef VERTEX_LAYOUT_H
#define VERTEX_LAYOUT_H

#include <vector>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

namespace Bind
{
    class VertexShaderProgram;

    class InputLayout : public Bindable
    {
        GLuint m_id;

    public:
        InputLayout(Graphics& gfx, std::vector<InputElementDesc> const& inputElementDesc,
                    VertexShaderProgram const* programWithInputSignature);
        ~InputLayout() override;
        void Bind() override;
    };
}

#endif
