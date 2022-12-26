#include "object/Sphere.hpp"
#include "util/Logger.h"

Sphere::Sphere(int numSegments, int numRings)
{
    m_mesh = ConstructSphere(numSegments, numRings);
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
    st.Apply(m_mesh);
    m_transform = Transform();
}
