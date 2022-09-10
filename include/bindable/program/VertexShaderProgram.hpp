#ifndef VERTEX_SHADER_PROGRAM_H
#define VERTEX_SHADER_PROGRAM_H

#include <string>

#include <glad/glad.h>

#include "bindable/Bindable.hpp"
#include "bindable/InputLayout.hpp"
#include "gfx/Graphics.hpp"

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
        GLWRPtr<GLWRVertexShader> m_program;
    };
}

#endif
