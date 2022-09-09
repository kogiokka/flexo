#ifndef PROGRAM_PIPELINE_H
#define PROGRAM_PIPELINE_H

#include <glad/glad.h>

#include "bindable/Bindable.hpp"
#include "bindable/program/FragmentShaderProgram.hpp"
#include "bindable/program/VertexShaderProgram.hpp"
#include "gfx/Graphics.hpp"

namespace Bind
{
    class ProgramPipeline : public Bindable
    {
        friend VertexShaderProgram;
        friend FragmentShaderProgram;

        GLuint m_id;

    public:
        ProgramPipeline(Graphics& gfx);
        ~ProgramPipeline();
        virtual void Bind() override;
    };
}

#endif
