#ifndef VERTEX_LAYOUT_H
#define VERTEX_LAYOUT_H

#include <vector>

#include "bindable/Bindable.hpp"
#include "gfx/Graphics.hpp"

namespace Bind
{
    class VertexShaderProgram;

    class InputLayout : public Bindable
    {
        GLuint m_id;

    public:
        InputLayout(Graphics& gfx, std::vector<GLWRInputElementDesc> const& inputElementDesc,
                    VertexShaderProgram const* programWithInputSignature);
        ~InputLayout() override;
        void Bind() override;
    };
}

#endif
