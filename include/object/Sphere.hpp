#ifndef SPHERE_H
#define SPHERE_H

#include "EditableMesh.hpp"
#include "Object.hpp"

struct TransformStack;

class Sphere : public Object
{
public:
    Sphere(int numSegments = 32, int numRings = 16);
    virtual ~Sphere() = default;
    void SetTransform(TransformStack stack);

    Mesh GenerateMesh() const override;
    Wireframe GenerateWireMesh() const override;

private:
    EditableMesh m_mesh;
};

#endif
