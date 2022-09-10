#include "bindable/Sampler.hpp"

namespace Bind
{
    Sampler::Sampler(Graphics& gfx, GLWRSamplerDesc samplerDesc, GLuint textureUnit)
        : Bindable(gfx)
        , m_unit(textureUnit)
    {
        m_gfx->CreateSampler(&samplerDesc, &m_sampler);
    }

    Sampler::~Sampler()
    {
    }

    void Sampler::Bind()
    {
        m_gfx->SetSamplers(m_unit, 1, &m_sampler);
    }
}
