#include "object/Cube.hpp"

Cube::Cube(float size)
{
    m_mesh = ConstructCube(size);
}

void Cube::ApplyTransform()
{
    auto st = GenerateTransformStack();
    st.Apply(m_mesh.positions);

    m_transform = Transform();
}
