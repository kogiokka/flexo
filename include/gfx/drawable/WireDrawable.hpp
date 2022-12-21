#ifndef WIRE_DRAWABLE_H
#define WIRE_DRAWABLE_H

#include "Wireframe.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/drawable/IndexedDrawable.hpp"

class WireDrawable : public IndexedDrawable
{
public:
    WireDrawable(Graphics& gfx, Wireframe const& wireframe);
    ~WireDrawable() override;
    void SetColor(float r, float g, float b);
    virtual void Update(Graphics& gfx) override;
};

#endif
