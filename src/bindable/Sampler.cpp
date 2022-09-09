#include "bindable/Sampler.hpp"

namespace Bind
{
    Sampler::Sampler(Graphics& gfx, GLWRSamplerDesc samplerDesc, GLuint textureUnit)
        : Bindable(gfx)
        , m_unit(textureUnit)
    {
        m_gfx->CreateSampler(m_id, samplerDesc);
    }

    Sampler::~Sampler()
    {
        m_gfx->DeleteSampler(m_id);
    }

    void Sampler::Bind()
    {
        m_gfx->SetSampler(m_unit, m_id);
    }
}
