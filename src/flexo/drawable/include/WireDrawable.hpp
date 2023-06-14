#ifndef WIRE_DRAWABLE_H
#define WIRE_DRAWABLE_H

#include "IndexedDrawable.hpp"
#include "Colors.hpp"

class Graphics;
struct Wireframe;

class WireDrawable : public IndexedDrawable
{
public:
    WireDrawable(Graphics& gfx, Wireframe const& wireframe);
    ~WireDrawable() override;
    void SetColor(Color color);
    virtual void Update(Graphics& gfx) override;
};

#endif
