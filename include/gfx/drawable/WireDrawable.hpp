#ifndef WIRE_DRAWABLE_H
#define WIRE_DRAWABLE_H

#include "gfx/drawable/IndexedDrawable.hpp"

class Graphics;
struct Wireframe;

class WireDrawable : public IndexedDrawable
{
public:
    WireDrawable(Graphics& gfx, Wireframe const& wireframe);
    ~WireDrawable() override;
};

#endif
