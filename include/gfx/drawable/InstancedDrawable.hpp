#ifndef INSTANCED_DRAWABLE_H
#define INSTANCED_DRAWABLE_H

#include "gfx/drawable/DrawableBase.hpp"

class Graphics;

class InstancedDrawable : public DrawableBase
{
public:
    InstancedDrawable();
    InstancedDrawable(InstancedDrawable const&) = delete;
    virtual ~InstancedDrawable() = default;
    virtual void Draw(Graphics& gfx) const override;

private:
    unsigned int m_vertexCountPerInstance;
    unsigned int m_instanceCount;
};

#endif
