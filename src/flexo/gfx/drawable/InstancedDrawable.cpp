#include "gfx/drawable/InstancedDrawable.hpp"
#include "gfx/Graphics.hpp"

InstancedDrawable::InstancedDrawable()
    : m_vertexCountPerInstance(0)
    , m_instanceCount(0)
{
}

void InstancedDrawable::Draw(Graphics& gfx) const
{
    gfx.DrawInstanced(m_vertexCountPerInstance, m_instanceCount);
}
