#include "object/Torus.hpp"

Torus::Torus(int majorSeg, int minorSeg, float majorRad, float minorRad)
{
    m_mesh = ConstructTorus(majorSeg, minorSeg, majorRad, minorRad);
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
