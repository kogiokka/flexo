#ifndef BIND_STEP_H
#define BIND_STEP_H

#include <memory>
#include <vector>

#include "gfx/bindable/Bindable.hpp"

class Renderer;
class Graphics;
class DrawableBase;

// Sequence of bindables
struct BindStep {
    void AddBindable(std::shared_ptr<Bind::Bindable> bind);
    void Submit(Renderer& renderer, DrawableBase& drawable);
    void Bind(Graphics& gfx) const;

    std::vector<std::shared_ptr<Bind::Bindable>> bindables;
};

#endif
