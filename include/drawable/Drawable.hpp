#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"
#include "bindable/VertexBuffer.hpp"
#include "drawable/DrawableBase.hpp"

class Drawable : public DrawableBase
{
    int m_id;
    Bind::VertexBuffer const* m_buffer;

public:
    Drawable();
    Drawable(Drawable const&) = delete;
    virtual ~Drawable() = default;

    virtual void Draw(Graphics& gfx) const override;
    virtual void Update(Graphics& gfx) override;
    virtual glm::mat4 GetTransformMatrix() const override = 0;
    void AddBind(std::shared_ptr<Bind::Bindable> bind) override;
};

#endif
