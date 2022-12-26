#ifndef PLANE_H
#define PLANE_H

#include "EditableMesh.hpp"
#include "Object.hpp"

class Plane : public Object
{
public:
    Plane();
    virtual ~Plane() = default;

    Mesh GenerateMesh() const override;
    Wireframe GenerateWireMesh() const override;
    void ApplyTransform() override;
};

#endif
