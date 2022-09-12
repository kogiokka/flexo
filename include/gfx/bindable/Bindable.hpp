#ifndef BINDABLE_H
#define BINDABLE_H

#include "gfx/Graphics.hpp"

namespace Bind
{
    class Bindable
    {
    protected:
        Graphics* m_gfx;

    public:
        Bindable(Graphics& gfx);
        virtual void Bind() = 0;
        virtual ~Bindable() = 0;
    };
}

#endif
