#ifndef GRID_H
#define GRID_H

#include "EditableMesh.hpp"
#include "Object.hpp"

class Grid : public Object
{
public:
    Grid(int numXDiv, int numYDiv, float size);
    virtual ~Grid() = default;

    void ApplyTransform() override;
};

#endif
