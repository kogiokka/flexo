#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/drawable/Drawable.hpp"

class LightSource : public Drawable
{
    struct UniformBlock {
        struct Frag {
            STD140_ALIGN glm::vec3 lightColor;
        };

        Frag frag;
    };

    UniformBlock m_ub;

public:
    LightSource(Graphics& gfx, Mesh const& mesh);
    ~LightSource() override;
    void Update(Graphics& gfx) override;
};

#endif
