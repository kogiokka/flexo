#include "object/Torus.hpp"

Torus::Torus(int majorSeg, int minorSeg, float majorRad, float minorRad)
    : Object(ObjectType_Torus)
{
    m_mesh = ConstructTorus(majorSeg, minorSeg, majorRad, minorRad);
}

void Torus::ApplyTransform()
{
    auto st = GenerateTransformStack();
    st.Apply(m_mesh.positions);

    m_transform = Transform();
}
