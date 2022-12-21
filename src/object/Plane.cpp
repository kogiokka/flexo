#include "object/Plane.hpp"

Plane::Plane()
{
    m_mesh = ConstructPlane();
}

Mesh Plane::GenerateMesh() const
{
    return m_mesh.GenerateMesh();
}

Wireframe Plane::GenerateWireMesh() const
{
    return m_mesh.GenerateWireframe();
}
