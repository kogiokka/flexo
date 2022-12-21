#include "object/Cube.hpp"

Cube::Cube()
{
    m_mesh = ConstructCube();
}

Mesh Cube::GenerateMesh() const
{
    return m_mesh.GenerateMesh();
}

Wireframe Cube::GenerateWireMesh() const
{
    return m_mesh.GenerateWireframe();
}
