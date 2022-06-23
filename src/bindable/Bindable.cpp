#include "bindable/Bindable.hpp"

namespace Bind
{
    Bindable::Bindable(Graphics& gfx)
    {
        gfx_ = &gfx;
    }

    Bindable::~Bindable() { }
}
