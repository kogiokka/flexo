#ifndef SAMPLER_H
#define SAMPLER_H

#include "gfx/Graphics.hpp"
#include "gfx/bindable/Bindable.hpp"

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
        GLWRPtr<IGLWRSampler> m_sampler;
    };
}

#endif
