#ifndef TORUS_H
#define TORUS_H

#include "EditableMesh.hpp"
#include "Object.hpp"

class Torus : public Object
{
public:
    Torus(int majorSeg = 48, int minorSeg = 12, float majorRad = 1.0f, float minorRad = 0.25f);
    virtual ~Torus() = default;

    void ApplyTransform() override;
};

#endif
