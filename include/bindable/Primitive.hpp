#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <glad/glad.h>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

namespace Bind
{
    class Primitive : public Bindable
    {
    protected:
        GLenum mode_;

    public:
        Primitive(Graphics& gfx, GLenum mode);
        ~Primitive() override;
        void Bind();
    };
}

#endif