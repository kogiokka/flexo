#include "bindable/VertexLayout.hpp"

#include "common/Logger.hpp"

VertexLayout::VertexLayout(Graphics& gfx, std::vector<AttributeDesc> const& attrDescs)
{
    gfx.CreateVertexLayout(id_, attrDescs.data(), attrDescs.size());
    Logger::info("size: %lu", attrDescs.size());
}

void VertexLayout::Bind(Graphics& gfx)
{
    gfx.SetVertexLayout(id_);
}
