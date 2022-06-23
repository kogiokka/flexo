#ifndef VERTEX_LAYOUT_H
#define VERTEX_LAYOUT_H

#include <vector>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

namespace Bind
{
    class VertexLayout : public Bindable
    {
        GLuint id_;

    public:
        VertexLayout(Graphics& gfx, std::vector<AttributeDesc> const& attrDescs);
        ~VertexLayout() override;
        void Bind() override;
    };
}

#endif
