#ifndef CUBE_H
#define CUBE_H

#include "EditableMesh.hpp"
#include "Object.hpp"

class Cube : public Object
{
public:
    Cube();
    virtual ~Cube() = default;

    Mesh GenerateMesh() const override;
    Wireframe GenerateWireMesh() const override;
    void ApplyTransform() override;

private:
    EditableMesh m_mesh;
};

#endif
