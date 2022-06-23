#include "bindable/VertexLayout.hpp"

namespace Bind
{
    VertexLayout::VertexLayout(Graphics& gfx, std::vector<AttributeDesc> const& attrDescs)
        : Bindable(gfx)
    {
        gfx_->CreateVertexLayout(id_, attrDescs.data(), attrDescs.size());
    }

    VertexLayout::~VertexLayout()
    {
        gfx_->DeleteVertexLayout(id_);
    }

    void VertexLayout::Bind()
    {
        gfx_->SetVertexLayout(id_);
    }
}
