#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include "EditableMesh.hpp"
#include "Object.hpp"

class LightSource : public Object
{
public:
    LightSource();
    virtual ~LightSource() = default;
    void SetPosition(float x, float y, float z);

    Mesh GenerateSolidMesh() const override;
    Wireframe GenerateWireMesh() const override;
    Mesh GenerateTexturedMesh() const override;

private:
    EditableMesh m_mesh;
};

#endif
