#include "Drawable.hpp"

void Drawable::Draw(Graphics& gfx) const
{
    for (auto& b : binds) {
        b->Bind(gfx);
    }
}
