#include "LightSource.hpp"

LightSource::LightSource()
{
    m_mesh = ConstructSphere(32, 16);
}

void LightSource::SetPosition(float x, float y, float z)
{
    EditableMesh::TransformStack tf;
    tf.PushTranslate(x, y, z);
    tf.PushScale(0.2f, 0.2f, 0.2f);
    tf.Apply(m_mesh);
}

Mesh LightSource::GenerateSolidMesh() const
{
    return m_mesh.GenerateMesh();
}

Mesh LightSource::GenerateTexturedMesh() const
{
    return m_mesh.GenerateMesh();
}

Wireframe LightSource::GenerateWireMesh() const
{
    return m_mesh.GenerateWireframe();
}
