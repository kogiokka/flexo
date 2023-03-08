#include "object/Sphere.hpp"
#include "util/Logger.h"

Sphere::Sphere(int numSegments, int numRings, float radius)
{
    m_mesh = ConstructSphere(numSegments, numRings, radius);
}

void Sphere::ApplyTransform()
{
    auto st = GenerateTransformStack();
    st.Apply(m_mesh.positions);
    m_transform = Transform();
}
