#include "drawable/Drawable.hpp"

void Drawable::Draw(Graphics& gfx) const
{
    for (auto& b : binds_) {
        b->Bind(gfx);
    }
}

void Drawable::Update(Graphics&) { }

void Drawable::AddBind(std::shared_ptr<Bind::Bindable> bind)
{
    binds_.push_back(bind);
}
