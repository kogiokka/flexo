#ifndef SPHERE_H
#define SPHERE_H

#include "EditableMesh.hpp"
#include "Object.hpp"

class Sphere : public Object
{
public:
    Sphere(int numSegments = 32, int numRings = 16);
    virtual ~Sphere() = default;
    void SetTransform(EditableMesh::TransformStack stack);

    Mesh GenerateSolidMesh() const override;
    Wireframe GenerateWireMesh() const override;
    Mesh GenerateTexturedMesh() const override;

private:
    EditableMesh m_mesh;
};

#endif
