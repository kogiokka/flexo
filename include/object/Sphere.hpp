#ifndef SPHERE_H
#define SPHERE_H

#include "EditableMesh.hpp"
#include "Object.hpp"

class Sphere : public Object
{
public:
    Sphere(int numSegments = 32, int numRings = 16);
    virtual ~Sphere() = default;

    Mesh GenerateMesh() const override;
    Wireframe GenerateWireMesh() const override;
    void ApplyTransform() override;
};

#endif
