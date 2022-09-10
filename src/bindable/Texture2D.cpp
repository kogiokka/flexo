#include "bindable/Texture2D.hpp"

#include "common/Logger.hpp"

namespace Bind
{
    Texture2D::~Texture2D()
    {
    }

    void Texture2D::Bind()
    {
        m_gfx->SetTexture2D(m_unit, 1, &m_texture);
    }

    template <>
    void Texture2D::DetermineDataType<float>(GLWRTexture2DDesc& desc) const
    {
        desc.dataType = GL_FLOAT;
    }

    template <>
    void Texture2D::DetermineDataType<unsigned char>(GLWRTexture2DDesc& desc) const
    {
        desc.dataType = GL_UNSIGNED_BYTE;
    }
}
