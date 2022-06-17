#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <glad/glad.h>

#include "bindable/Bindable.hpp"
#include "Graphics.hpp"

class Primitive : public Bindable
{
protected:
    GLenum mode_;

public:
    Primitive(GLenum mode);
    void Bind(Graphics& gfx) override;
};

#endif
