#include "object/Torus.hpp"

Torus::Torus(int majorSeg, int minorSeg)
{
    m_mesh = ConstructTorus(majorSeg, minorSeg, 1.0f, 0.25f);
}

Mesh Torus::GenerateMesh() const
{
    return m_mesh.GenerateMesh();
}

Wireframe Torus::GenerateWireMesh() const
{
    return m_mesh.GenerateWireframe();
}

void Torus::ApplyTransform()
{
    auto st = GenerateTransformStack();
    st.Apply(m_mesh.positions);

    m_transform = Transform();
}
