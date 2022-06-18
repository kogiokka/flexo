#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <memory>
#include <vector>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"
#include "bindable/IndexBuffer.hpp"

class Drawable
{
public:
    Drawable() = default;
    Drawable(Drawable const&) = delete;
    virtual ~Drawable() = default;

    void Draw(Graphics& gfx) const;
    void AddBind(std::unique_ptr<Bindable> bind);

private:
    IndexBuffer const* indexBuffer_;
    std::vector<std::unique_ptr<Bindable>> binds;
};

#endif
