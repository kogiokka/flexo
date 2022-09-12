#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "gfx/Graphics.hpp"
#include "gfx/bindable/Bindable.hpp"
#include "gfx/bindable/VertexBuffer.hpp"
#include "gfx/drawable/DrawableBase.hpp"

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
