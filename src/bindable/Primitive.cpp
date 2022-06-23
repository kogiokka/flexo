#include "bindable/Primitive.hpp"

namespace Bind
{
    Primitive::Primitive(Graphics&, GLenum mode)
        : mode_(mode)
    {
    }

    void Primitive::Bind(Graphics& gfx)
    {
        gfx.SetPrimitive(mode_);
    }
}
