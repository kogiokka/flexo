#include "bindable/VertexLayout.hpp"

namespace Bind
{
    VertexLayout::VertexLayout(Graphics& gfx, std::vector<AttributeDesc> const& attrDescs)
    {
        gfx.CreateVertexLayout(id_, attrDescs.data(), attrDescs.size());
    }

    void VertexLayout::Bind(Graphics& gfx)
    {
        gfx.SetVertexLayout(id_);
    }
}
