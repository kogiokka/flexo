#ifndef BINDABLE_H
#define BINDABLE_H

#include "Graphics.hpp"

namespace Bind
{
    class Bindable
    {
    public:
        virtual void Bind(Graphics& gfx) = 0;
        virtual ~Bindable() = default;
    };
}

#endif
