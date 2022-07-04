#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include <glm/glm.hpp>

#include "Graphics.hpp"
#include "Mesh.hpp"
#include "drawable/Drawable.hpp"

class LightSource : public Drawable
{
    struct UniformBlock {
        struct Frag {
            STD140_ALIGN glm::vec3 lightColor;
        };

        Frag frag;
    };

    GLuint m_count;
    UniformBlock m_ub;

public:
    LightSource(Graphics& gfx, Mesh const& mesh);
    void Draw(Graphics& gfx) const override;
    void Update(Graphics& gfx) override;
    glm::mat4 GetTransformMatrix() const override;
};

#endif
