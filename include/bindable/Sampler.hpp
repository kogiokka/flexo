#ifndef SAMPLER_H
#define SAMPLER_H

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

namespace Bind
{
    class Sampler : public Bindable
    {
    public:
        Sampler(Graphics& gfx, SamplerDesc samplerDesc, GLuint textureUnit);
        ~Sampler() override;
        void Bind() override;

    protected:
        GLuint m_id;
        GLuint m_unit;
    };
}

#endif
