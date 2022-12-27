#include "object/Grid.hpp"

Grid::Grid(int numXDiv, int numYDiv, float size)
{
    m_mesh = ConstructGrid(numXDiv, numYDiv, size);
}

Mesh Grid::GenerateMesh() const
{
    return m_mesh.GenerateMesh();
}

Wireframe Grid::GenerateWireMesh() const
{
    return m_mesh.GenerateWireframe();
}

void Grid::ApplyTransform()
{
    auto st = GenerateTransformStack();
    st.Apply(m_mesh.positions);

    m_transform = Transform();
}
