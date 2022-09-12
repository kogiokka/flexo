#ifndef INDEXED_DRAWABLE_H
#define INDEXED_DRAWABLE_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "gfx/Graphics.hpp"
#include "gfx/bindable/Bindable.hpp"
#include "gfx/bindable/IndexBuffer.hpp"
#include "gfx/drawable/DrawableBase.hpp"

class IndexedDrawable : public DrawableBase
{
    Bind::IndexBuffer const* m_buffer;

public:
    IndexedDrawable();
    IndexedDrawable(IndexedDrawable const&) = delete;
    virtual ~IndexedDrawable() = default;

    virtual void Bind(Graphics& gfx) const override;
    virtual void Update(Graphics& gfx) override;
    void AddBind(std::shared_ptr<Bind::Bindable> bind) override;
};

#endif
