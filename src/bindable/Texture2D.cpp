#include "bindable/Texture2D.hpp"

#include "common/Logger.hpp"

namespace Bind
{
    Texture2D::~Texture2D()
    {
        m_gfx->DeleteTexture(m_id);
    }

    void Texture2D::Bind()
    {
        m_gfx->SetTexture(GL_TEXTURE_2D, m_id, m_unit);
    }

    template <>
    void Texture2D::DetermineDataType<float>(Texture2dDesc& desc) const
    {
        desc.dataType = GL_FLOAT;
    }

    template <>
    void Texture2D::DetermineDataType<unsigned char>(Texture2dDesc& desc) const
    {
        desc.dataType = GL_UNSIGNED_BYTE;
    }
}
