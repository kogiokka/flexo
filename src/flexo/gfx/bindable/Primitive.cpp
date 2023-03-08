#include "gfx/bindable/Primitive.hpp"

namespace Bind
{
    Primitive::Primitive(Graphics&, GLenum mode)
        : m_mode(mode)
    {
    }

    Primitive::~Primitive()
    {
    }

    void Primitive::Bind(Graphics& gfx)
    {
        gfx.SetPrimitive(m_mode);
    }
}
