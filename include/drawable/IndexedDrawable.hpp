#ifndef INDEXED_DRAWABLE_H
#define INDEXED_DRAWABLE_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"
#include "bindable/IndexBuffer.hpp"
#include "drawable/DrawableBase.hpp"

class IndexedDrawable : public DrawableBase
{
    Bind::IndexBuffer const* m_buffer;

public:
    IndexedDrawable();
    IndexedDrawable(IndexedDrawable const&) = delete;
    virtual ~IndexedDrawable() = default;

    virtual void Draw(Graphics& gfx) const override;
    virtual void Update(Graphics& gfx) override;
    virtual glm::mat4 GetTransformMatrix() const override = 0;
    void AddBind(std::shared_ptr<Bind::Bindable> bind) override;
};

#endif

