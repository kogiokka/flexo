#include "bindable/Texture2D.hpp"

void Texture2D::Bind(Graphics& gfx)
{
    gfx.SetTexture(GL_TEXTURE_2D, id_);
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
