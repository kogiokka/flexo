#ifndef TORUS_H
#define TORUS_H

#include "EditableMesh.hpp"
#include "Object.hpp"

class Torus : public Object
{
public:
    Torus(int majorSeg = 48, int minorSeg = 12);
    virtual ~Torus() = default;

    Mesh GenerateMesh() const override;
    Wireframe GenerateWireMesh() const override;
    void ApplyTransform() override;
};

#endif
