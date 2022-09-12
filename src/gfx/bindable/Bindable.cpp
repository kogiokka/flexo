#include "gfx/bindable/Bindable.hpp"

namespace Bind
{
    Bindable::Bindable(Graphics& gfx)
    {
        m_gfx = &gfx;
    }

    Bindable::~Bindable()
    {
    }
}
