#ifndef VERTEX_SHADER_PROGRAM_H
#define VERTEX_SHADER_PROGRAM_H

#include <memory>
#include <string>

#include <glad/glad.h>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"
#include "bindable/program/ProgramPipeline.hpp"

namespace Bind
{
    class VertexShaderProgram : public Bindable
    {
        GLuint m_id;
        ProgramPipeline* m_pipeline;

    public:
        VertexShaderProgram(Graphics& gfx, std::string const& filename, ProgramPipeline* pipeline);
        ~VertexShaderProgram() override;
        virtual void Bind() override;
    };
}

#endif
