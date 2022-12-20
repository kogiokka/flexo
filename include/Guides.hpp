#ifndef GUIDES_H
#define GUIDES_H

#include "EditableMesh.hpp"
#include "Object.hpp"

class Guides : public Object
{
public:
    Guides();
    virtual ~Guides() = default;

    void GenerateDrawables(Graphics& gfx) override;
    Mesh GenerateSolidMesh() const override;
    Wireframe GenerateWireMesh() const override;
    Mesh GenerateTexturedMesh() const override;

private:
    EditableMesh m_mesh;
};

#endif
