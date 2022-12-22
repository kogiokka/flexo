#include <memory>

#include "gfx/drawable/DrawableBase.hpp"
#include "gfx/drawable/WireDrawable.hpp"
#include "object/Guides.hpp"
#include "TransformStack.hpp"

Guides::Guides()
{
    SetViewFlags(ObjectViewFlag_Wire);

    m_mesh = ConstructGrid(180, 180);

    TransformStack ts;
    ts.PushScale(90.0f, 90.0f, 1.0f);
    ts.Apply(m_mesh);
}

void Guides::GenerateDrawables(Graphics& gfx)
{
    auto wire = std::make_shared<WireDrawable>(gfx, GenerateWireMesh());
    wire->SetColor(0.3f, 0.3f, 0.3f);
    wire->SetVisible(m_isVisible);

    m_wire = wire;
}

Object::DrawList const& Guides::GetDrawList()
{
    m_drawlist.clear();
    m_drawlist.push_back(m_wire);
    return m_drawlist;
}

Mesh Guides::GenerateMesh() const
{
    return m_mesh.GenerateMesh();
}

Wireframe Guides::GenerateWireMesh() const
{
    return m_mesh.GenerateWireframe();
}
