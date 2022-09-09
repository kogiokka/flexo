#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "bindable/Bindable.hpp"
#include "bindable/VertexBuffer.hpp"
#include "drawable/DrawableBase.hpp"
#include "gfx/Graphics.hpp"

class Drawable : public DrawableBase
{
    Bind::VertexBuffer const* m_buffer;

public:
    Drawable();
    Drawable(Drawable const&) = delete;
    virtual ~Drawable() = default;

    virtual void Bind(Graphics& gfx) const override;
    virtual void Update(Graphics& gfx) override;
    void AddBind(std::shared_ptr<Bind::Bindable> bind) override;
};

#endif
