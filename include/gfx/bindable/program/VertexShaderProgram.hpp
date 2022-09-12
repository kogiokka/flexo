#ifndef VERTEX_SHADER_PROGRAM_H
#define VERTEX_SHADER_PROGRAM_H

#include <string>

#include <glad/glad.h>

#include "gfx/Graphics.hpp"
#include "gfx/bindable/Bindable.hpp"
#include "gfx/bindable/InputLayout.hpp"

namespace Bind
{
    class VertexShaderProgram : public Bindable
    {
        friend InputLayout;

    public:
        VertexShaderProgram(Graphics& gfx, std::string const& filename);
        ~VertexShaderProgram() override;
        virtual void Bind() override;

    private:
        GLWRPtr<IGLWRVertexShader> m_program;
    };
}

#endif
