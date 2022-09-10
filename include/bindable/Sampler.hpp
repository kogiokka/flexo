#ifndef SAMPLER_H
#define SAMPLER_H

#include "bindable/Bindable.hpp"
#include "gfx/Graphics.hpp"

namespace Bind
{
    class Sampler : public Bindable
    {
    public:
        Sampler(Graphics& gfx, GLWRSamplerDesc samplerDesc, GLuint textureUnit);
        ~Sampler() override;
        void Bind() override;

    protected:
        GLuint m_unit;
        GLWRPtr<GLWRSampler> m_sampler;
    };
}

#endif
