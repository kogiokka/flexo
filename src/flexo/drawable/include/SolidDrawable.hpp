#ifndef SOLID_DRAWABLE_H
#define SOLID_DRAWABLE_H

#include "Drawable.hpp"
#include "gfx/Mesh.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/UniformBlock.hpp"

class SolidDrawable : public Drawable
{
public:
    SolidDrawable(Graphics& gfx, Mesh const& mesh);
    ~SolidDrawable() override;
    virtual void Update(Graphics& gfx) override;
};

#endif
