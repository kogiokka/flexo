#ifndef WIRE_DRAWABLE_H
#define WIRE_DRAWABLE_H

#include <vector>

#include <glm/glm.hpp>

#include "Wireframe.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/UniformBlock.hpp"
#include "gfx/bindable/IndexBuffer.hpp"
#include "gfx/drawable/IndexedDrawable.hpp"

class WireDrawable : public IndexedDrawable
{
public:
    WireDrawable(Graphics& gfx, Wireframe const& wireframe);
    ~WireDrawable() override;
    void SetColor(float r, float g, float b);
    void Update(Graphics& gfx) override;

private:
    UniformBlock m_ub;
};

#endif
