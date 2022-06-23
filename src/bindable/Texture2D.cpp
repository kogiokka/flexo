#include "bindable/Texture2D.hpp"

#include "common/Logger.hpp"

namespace Bind
{
    Texture2D::~Texture2D()
    {
        gfx_->DeleteTexture(id_);
    }

    void Texture2D::Bind()
    {
        gfx_->SetTexture(GL_TEXTURE_2D, id_, unit_);
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
