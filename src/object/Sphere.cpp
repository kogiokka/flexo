#include "object/Sphere.hpp"
#include "util/Logger.h"

Sphere::Sphere(int numSegments, int numRings, float radius)
{
    m_mesh = ConstructSphere(numSegments, numRings, radius);
}

Mesh Sphere::GenerateMesh() const
{
    return m_mesh.GenerateMesh();
}

Wireframe Sphere::GenerateWireMesh() const
{
    return m_mesh.GenerateWireframe();
}

void Sphere::ApplyTransform()
{
    auto st = GenerateTransformStack();
    st.Apply(m_mesh.positions);
    m_transform = Transform();
}
