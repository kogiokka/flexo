#ifndef PLANE_H
#define PLANE_H

#include "EditableMesh.hpp"
#include "Object.hpp"

class Plane : public Object
{
public:
    Plane(float size = 2.0f);
    virtual ~Plane() = default;

    void ApplyTransform() override;
};

#endif
