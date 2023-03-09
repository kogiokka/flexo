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
    void ApplyTransform() override;
    Object::DrawList const& GetDrawList() override;

private:
    EditableMesh m_fine;
    EditableMesh m_coarse;
    EditableMesh m_xAxis;
    EditableMesh m_yAxis;
};

#endif