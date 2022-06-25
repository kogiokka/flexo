#include "drawable/Drawable.hpp"

void Drawable::Draw(Graphics&) const
{
    for (auto& b : m_binds) {
        b->Bind();
    }
}

void Drawable::Update(Graphics&) { }

void Drawable::AddBind(std::shared_ptr<Bind::Bindable> bind)
{
    m_binds.push_back(bind);
}
