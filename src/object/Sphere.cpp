#include "object/Sphere.hpp"
#include "TransformStack.hpp"

Sphere::Sphere(int numSegments, int numRings)
{
    m_mesh = ConstructSphere(numSegments, numRings);
}

void Sphere::SetTransform(TransformStack stack)
{
    stack.Apply(m_mesh);
}

Mesh Sphere::GenerateMesh() const
{
    return m_mesh.GenerateMesh();
}

Wireframe Sphere::GenerateWireMesh() const
{
    return m_mesh.GenerateWireframe();
}
