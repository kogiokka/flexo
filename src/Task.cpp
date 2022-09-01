#include "Task.hpp"
#include "drawable/DrawableBase.hpp"

void Task::AddBindable(std::shared_ptr<Bind::Bindable> bind)
{
    mBinds.push_back(bind);
}

void Task::Execute(Graphics& gfx) const
{
    for (auto const& bind : mBinds) {
        bind->Bind();
    }

    mDrawable->Update(gfx);
    mDrawable->Bind(gfx);
}
