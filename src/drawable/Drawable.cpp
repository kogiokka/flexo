#include "drawable/Drawable.hpp"

void Drawable::Draw(Graphics&) const
{
    for (auto& b : binds_) {
        b->Bind();
    }
}

void Drawable::Update(Graphics&) { }

void Drawable::AddBind(std::shared_ptr<Bind::Bindable> bind)
{
    binds_.push_back(bind);
}
