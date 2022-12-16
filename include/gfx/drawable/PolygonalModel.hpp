#ifndef POLYGONAL_MODEL_H
#define POLYGONAL_MODEL_H

#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/drawable/Drawable.hpp"

class PolygonalModel : public Drawable
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
    PolygonalModel(Graphics& gfx, Mesh const& mesh);
    ~PolygonalModel() override;
    void Update(Graphics& gfx) override;
    std::string GetName() const override;
};

#endif
