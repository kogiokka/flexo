#ifndef VERTEX_SHADER_PROGRAM_H
#define VERTEX_SHADER_PROGRAM_H

#include <memory>
#include <string>

#include <glad/glad.h>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"
#include "bindable/InputLayout.hpp"

namespace Bind
{
    class ProgramPipeline;

    class VertexShaderProgram : public Bindable
    {
        friend InputLayout;

    public:
        VertexShaderProgram(Graphics& gfx, std::string const& filename, ProgramPipeline const& pipeline);
        ~VertexShaderProgram() override;
        virtual void Bind() override;

    private:
        GLuint m_id;
        ProgramPipeline const& m_pipeline;
    };
}

#endif
