#include "object/Grid.hpp"

Grid::Grid(int numXDiv, int numYDiv, float size)
{
    m_mesh = ConstructGrid(numXDiv, numYDiv, size);
}

void Grid::ApplyTransform()
{
    auto st = GenerateTransformStack();
    st.Apply(m_mesh.positions);

    m_transform = Transform();
}
