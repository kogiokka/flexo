#include "gfx/drawable/Drawable.hpp"
#include "gfx/Graphics.hpp"

Drawable::Drawable()
{
}

void Drawable::Draw(Graphics& gfx) const
{
    gfx.Draw(m_vertCount);
}
