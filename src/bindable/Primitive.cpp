#include "bindable/Primitive.hpp"

Primitive::Primitive(GLenum mode)
    : mode_(mode)
{
}

void Primitive::Bind(Graphics& gfx)
{
    gfx.SetPrimitive(mode_);
}
