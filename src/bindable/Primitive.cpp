#include "bindable/Primitive.hpp"

namespace Bind
{
    Primitive::Primitive(Graphics& gfx, GLenum mode)
        : Bindable(gfx)
        , m_mode(mode)
    {
    }

    Primitive::~Primitive() { }

    void Primitive::Bind()
    {
        m_gfx->SetPrimitive(m_mode);
    }
}
