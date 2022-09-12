#ifndef FRAGMENT_SHADER_PROGRAM_H
#define FRAGMENT_SHADER_PROGRAM_H

#include <string>

#include <glad/glad.h>

#include "gfx/Graphics.hpp"
#include "gfx/bindable/Bindable.hpp"

namespace Bind
{
    class FragmentShaderProgram : public Bindable
    {
    public:
        FragmentShaderProgram(Graphics& gfx, std::string const& filename);
        ~FragmentShaderProgram() override;
        virtual void Bind() override;

    private:
        GLWRPtr<GLWRFragmentShader> m_program;
    };
}

#endif
