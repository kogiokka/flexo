#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <memory>
#include <vector>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

class Drawable
{
public:
    Drawable() = default;
    Drawable(Drawable const&) = delete;
    virtual ~Drawable() = default;

    virtual void Draw(Graphics& gfx) const;
    virtual void Update(Graphics& gfx);
    void AddBind(std::shared_ptr<Bind::Bindable> bind);

protected:
    std::vector<std::shared_ptr<Bind::Bindable>> m_binds;
};

#endif

