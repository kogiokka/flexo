#ifndef SOLID_DRAWABLE_H
#define SOLID_DRAWABLE_H

#include "gfx/Graphics.hpp"
#include "gfx/drawable/Drawable.hpp"

class SolidDrawable : public Drawable
{
    struct UniformBlock {
        struct Frag {
            struct Light {
                STD140_ALIGN glm::vec3 position;
                STD140_ALIGN glm::vec3 ambient;
                STD140_ALIGN glm::vec3 diffusion;
                STD140_ALIGN glm::vec3 specular;
            };

            struct Material {
                STD140_ALIGN glm::vec3 ambient;
                STD140_ALIGN glm::vec3 diffusion;
                STD140_ALIGN glm::vec3 specular;
                float shininess;
            };

            Light light;
            Material material;
            STD140_ALIGN glm::vec3 viewPos;
        };

        Frag frag;
    };

    UniformBlock m_ub;

public:
    SolidDrawable(Graphics& gfx, Mesh const& mesh);
    ~SolidDrawable() override;
    void Update(Graphics& gfx) override;
};

#endif
