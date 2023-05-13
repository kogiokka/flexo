#include "object/Sphere.hpp"
#include "log/Logger.h"

Sphere::Sphere(int numSegments, int numRings, float radius)
    : Object(ObjectType_Sphere)
{
    m_mesh = ConstructSphere(numSegments, numRings, radius);
}

void Sphere::ApplyTransform()
{
    auto st = GenerateTransformStack();
    st.Apply(m_mesh.positions);
    m_transform = Transform();
}
