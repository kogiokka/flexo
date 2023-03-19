#ifndef DRAWABLE_BASE_H
#define DRAWABLE_BASE_H

class Graphics;

class DrawableBase
{
public:
    virtual void Draw(Graphics& gfx) const = 0;
    virtual void Update(Graphics& gfx) = 0;
};

#endif
