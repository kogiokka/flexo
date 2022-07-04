#ifndef FRAGMENT_SHADER_PROGRAM_H
#define FRAGMENT_SHADER_PROGRAM_H

#include <memory>
#include <string>

#include <glad/glad.h>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

namespace Bind
{
    class FragmentShaderProgram : public Bindable
    {
        GLuint m_id;
        GLuint m_pipeline;

    public:
        FragmentShaderProgram(Graphics& gfx, std::string const& filename, GLuint pipeline);
        ~FragmentShaderProgram() override;
        virtual void Bind() override;
    };
}

#endif
