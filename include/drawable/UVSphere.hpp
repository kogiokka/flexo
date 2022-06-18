#ifndef CUBE_H
#define CUBE_H

#include "Drawable.hpp"

class UVSphere : public Drawable
{
    GLuint count_;

public:
    UVSphere(Graphics& gfx);
    void Draw(Graphics& gfx) const override;
};

#endif
