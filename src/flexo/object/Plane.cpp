#include "object/Plane.hpp"

Plane::Plane(float size)
    : Object(ObjectType_Plane)
{
    m_mesh = ConstructPlane(size);
}

void Plane::ApplyTransform()
{
    auto st = GenerateTransformStack();
    st.Apply(m_mesh.positions);
    m_transform = Transform();
}
