#ifndef SOLID_DRAWABLE_H
#define SOLID_DRAWABLE_H

#include "gfx/Graphics.hpp"
#include "gfx/UniformBlock.hpp"
#include "gfx/drawable/Drawable.hpp"

class SolidDrawable : public Drawable
{
public:
    SolidDrawable(Graphics& gfx, Mesh const& mesh);
    ~SolidDrawable() override;
    void Update(Graphics& gfx) override;

private:
    UniformBlock m_ublight;
    UniformBlock m_ubmat;
    UniformBlock m_ubo;
};

#endif
