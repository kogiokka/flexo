#ifndef INSTANCED_DRAWABLE_H
#define INSTANCED_DRAWABLE_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"
#include "bindable/VertexBuffer.hpp"
#include "drawable/DrawableBase.hpp"

class InstancedDrawable : public DrawableBase
{
    Bind::VertexBuffer const* m_instance;
    Bind::VertexBuffer const* m_perInstanceData;

public:
    InstancedDrawable();
    InstancedDrawable(InstancedDrawable const&) = delete;
    virtual ~InstancedDrawable() = default;

    virtual void Bind(Graphics& gfx) const override;
    virtual void Update(Graphics& gfx) override;
    void AddBind(std::shared_ptr<Bind::Bindable> bind) override;
};

#endif
