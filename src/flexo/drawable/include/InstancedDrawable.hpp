#ifndef INSTANCED_DRAWABLE_H
#define INSTANCED_DRAWABLE_H

#include "Drawable.hpp"

class Graphics;

class InstancedDrawable : public Drawable
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
