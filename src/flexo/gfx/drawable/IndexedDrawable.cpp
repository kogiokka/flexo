#include "gfx/drawable/IndexedDrawable.hpp"
#include "gfx/Graphics.hpp"

IndexedDrawable::IndexedDrawable()
    : m_indexCount(0)
{
}

void IndexedDrawable::Draw(Graphics& gfx) const
{
    gfx.DrawIndexed(m_indexCount);
}
