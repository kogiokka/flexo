#ifndef TASK_H
#define TASK_H

#include <memory>
#include <vector>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

class DrawableBase;

struct Task {
    std::vector<std::shared_ptr<Bind::Bindable>> mBinds;
    DrawableBase* mDrawable;
    void Execute(Graphics& gfx) const;
    void AddBindable(std::shared_ptr<Bind::Bindable> bind);
};

#endif
