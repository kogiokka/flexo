#include "object/Plane.hpp"

Plane::Plane()
{
    m_mesh = ConstructPlane();
}

Mesh Plane::GenerateMesh() const
{
    return m_mesh.GenerateMesh();
}

Wireframe Plane::GenerateWireMesh() const
{
    return m_mesh.GenerateWireframe();
}

void Plane::ApplyTransform()
{
    auto st = GenerateTransformStack();
    st.Apply(m_mesh.positions);
    m_transform = Transform();
}
