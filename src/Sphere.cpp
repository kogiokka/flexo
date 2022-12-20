#include "Sphere.hpp"

Sphere::Sphere(int numSegments, int numRings)
{
    m_mesh = ConstructSphere(numSegments, numRings);
}

void Sphere::SetTransform(EditableMesh::TransformStack stack)
{
    stack.Apply(m_mesh);
}

Mesh Sphere::GenerateSolidMesh() const
{
    return m_mesh.GenerateMesh();
}

Mesh Sphere::GenerateTexturedMesh() const
{
    return m_mesh.GenerateMesh();
}

Wireframe Sphere::GenerateWireMesh() const
{
    return m_mesh.GenerateWireframe();
}
