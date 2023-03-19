#include "IndexedDrawable.hpp"
#include "gfx/Graphics.hpp"

IndexedDrawable::IndexedDrawable()
    : m_indexCount(0)
{
}

void IndexedDrawable::Draw(Graphics& gfx) const
{
    if (!IsVisible()) {
        return;
    }

    UpdateUniformBuffers(gfx);
    for (auto const& b : m_binds) {
        b->Bind(gfx);
    }

    gfx.DrawIndexed(m_indexCount);
}
