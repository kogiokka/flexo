#ifndef WIRE_DRAWABLE_H
#define WIRE_DRAWABLE_H

#include <vector>

#include <glm/glm.hpp>

#include "Wireframe.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/bindable/IndexBuffer.hpp"
#include "gfx/drawable/IndexedDrawable.hpp"

class WireDrawable : public IndexedDrawable
{
    struct UniformBlock {
        struct Frag {
            STD140_ALIGN glm::vec3 color;
        };

        Frag frag;
    };

    UniformBlock m_ub;

public:
    WireDrawable(Graphics& gfx, Wireframe const& wireframe);
    ~WireDrawable() override;
    void Update(Graphics& gfx) override;
};

#endif
