#include "gfx/bindable/Sampler.hpp"

namespace Bind
{
    Sampler::Sampler(Graphics& gfx, GLWRSamplerDesc samplerDesc, GLuint textureUnit)
        : m_unit(textureUnit)
    {
        gfx.CreateSampler(&samplerDesc, &m_sampler);
    }

    Sampler::~Sampler()
    {
    }

    void Sampler::Bind(Graphics& gfx)
    {
        gfx.SetSamplers(m_unit, 1, m_sampler.GetAddressOf());
    }
}
