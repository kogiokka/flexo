#include "bindable/VertexLayout.hpp"

namespace Bind
{
    VertexLayout::VertexLayout(Graphics& gfx, std::vector<AttributeDesc> const& attrDescs)
        : Bindable(gfx)
    {
        m_gfx->CreateVertexLayout(m_id, attrDescs.data(), attrDescs.size());
    }

    VertexLayout::~VertexLayout()
    {
        m_gfx->DeleteVertexLayout(m_id);
    }

    void VertexLayout::Bind()
    {
        m_gfx->SetVertexLayout(m_id);
    }
}
