#ifndef FRAGMENT_SHADER_PROGRAM_H
#define FRAGMENT_SHADER_PROGRAM_H

#include <memory>
#include <string>

#include <glad/glad.h>

#include "bindable/Bindable.hpp"
#include "gfx/Graphics.hpp"

namespace Bind
{
    class ProgramPipeline;

    class FragmentShaderProgram : public Bindable
    {
    public:
        FragmentShaderProgram(Graphics& gfx, std::string const& filename, ProgramPipeline const& pipeline);
        ~FragmentShaderProgram() override;
        virtual void Bind() override;

    private:
        GLuint m_id;
        ProgramPipeline const& m_pipeline;
    };
}

#endif
