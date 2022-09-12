#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <glad/glad.h>

#include "gfx/Graphics.hpp"
#include "gfx/bindable/Bindable.hpp"

namespace Bind
{
    class Primitive : public Bindable
    {
    protected:
        GLenum m_mode;

    public:
        Primitive(Graphics& gfx, GLenum mode);
        ~Primitive() override;
        void Bind();
    };
}

#endif
