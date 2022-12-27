#include "object/Cube.hpp"

Cube::Cube(float size)
{
    m_mesh = ConstructCube(size);
}

Mesh Cube::GenerateMesh() const
{
    return m_mesh.GenerateMesh();
}

Wireframe Cube::GenerateWireMesh() const
{
    return m_mesh.GenerateWireframe();
}

void Cube::ApplyTransform()
{
    auto st = GenerateTransformStack();
    st.Apply(m_mesh.positions);

    m_transform = Transform();
}
