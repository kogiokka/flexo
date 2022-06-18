#include <utility>

#include "Drawable.hpp"

void Drawable::Draw(Graphics& gfx) const
{
    for (auto& b : binds_) {
        b->Bind(gfx);
    }
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind)
{
    binds_.push_back(std::move(bind));
}
