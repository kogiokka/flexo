#include "bindable/Texture2D.hpp"

void Texture2D::Bind(Graphics& gfx)
{
    gfx.SetTexture(GL_TEXTURE_2D, id_);
}
