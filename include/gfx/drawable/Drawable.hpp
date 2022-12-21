#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "gfx/drawable/DrawableBase.hpp"

class Graphics;

class Drawable : public DrawableBase
{
public:
    Drawable();
    Drawable(Drawable const&) = delete;
    virtual ~Drawable() = default;
    virtual void Draw(Graphics& gfx) const override;

protected:
    unsigned int m_vertCount;
};

#endif
