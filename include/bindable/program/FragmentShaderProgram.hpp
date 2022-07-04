#ifndef FRAGMENT_SHADER_PROGRAM_H
#define FRAGMENT_SHADER_PROGRAM_H

#include <memory>
#include <string>

#include <glad/glad.h>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"
#include "bindable/program/ProgramPipeline.hpp"

namespace Bind
{
    class FragmentShaderProgram : public Bindable
    {
        GLuint m_id;
        ProgramPipeline* m_pipeline;

    public:
        FragmentShaderProgram(Graphics& gfx, std::string const& filename, ProgramPipeline* pipeline);
        ~FragmentShaderProgram() override;
        virtual void Bind() override;
    };
}

#endif
