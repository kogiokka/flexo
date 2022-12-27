#ifndef CUBE_H
#define CUBE_H

#include "EditableMesh.hpp"
#include "Object.hpp"

class Cube : public Object
{
public:
    Cube(float size = 2.0f);
    virtual ~Cube() = default;

    Mesh GenerateMesh() const override;
    Wireframe GenerateWireMesh() const override;
    void ApplyTransform() override;
};

#endif
