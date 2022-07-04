#ifndef PROGRAM_PIPELINE_H
#define PROGRAM_PIPELINE_H

#include <glad/glad.h>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

namespace Bind
{
    class ProgramPipeline : public Bindable
    {
        GLuint m_id;

    public:
        ProgramPipeline(Graphics& gfx);
        ~ProgramPipeline();
        virtual void Bind() override;
        GLuint GetId() const;
    };
}

#endif
