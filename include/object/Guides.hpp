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
    Mesh GenerateMesh() const override;
    Wireframe GenerateWireMesh() const override;
    Object::DrawList const& GetDrawList() override;

private:
    EditableMesh m_grid;
    EditableMesh m_xAxis;
    EditableMesh m_yAxis;
};

#endif
