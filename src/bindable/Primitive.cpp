#include "bindable/Primitive.hpp"

namespace Bind
{
    Primitive::Primitive(Graphics& gfx, GLenum mode)
        : Bindable(gfx)
        , mode_(mode)
    {
    }

    Primitive::~Primitive() { }

    void Primitive::Bind()
    {
        gfx_->SetPrimitive(mode_);
    }
}
