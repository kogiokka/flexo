#include <memory>

#include "Guides.hpp"
#include "gfx/drawable/DrawableBase.hpp"
#include "gfx/drawable/WireDrawable.hpp"

Guides::Guides()
{
    m_mesh = ConstructGrid(180, 180);

    EditableMesh::TransformStack tf;
    tf.PushScale(180.0f, 180.0f, 1.0f);
    tf.Apply(m_mesh);
}

void Guides::GenerateDrawables(Graphics& gfx)
{
    auto wire = std::make_shared<WireDrawable>(gfx, GenerateWireMesh());
    wire->SetColor(0.3f, 0.3f, 0.3f);

    auto const& drawable = std::dynamic_pointer_cast<DrawableBase>(wire);
    drawable->SetVisible(m_isVisible);
    m_drawables.push_back(drawable);
}

Mesh Guides::GenerateSolidMesh() const
{
    return m_mesh.GenerateMesh();
}

Mesh Guides::GenerateTexturedMesh() const
{
    return m_mesh.GenerateMesh();
}

Wireframe Guides::GenerateWireMesh() const
{
    return m_mesh.GenerateWireframe();
}
