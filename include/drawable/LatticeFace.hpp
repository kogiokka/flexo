#ifndef LATTICE_FACE_H
#define LATTICE_FACE_H

#include <glm/glm.hpp>

#include "Graphics.hpp"
#include "Mesh.hpp"
#include "drawable/Drawable.hpp"

class LatticeFace : public Drawable
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
    LatticeFace(Graphics& gfx, Mesh const& mesh);
    ~LatticeFace() override;
    void Update(Graphics& gfx) override;
    glm::mat4 GetTransformMatrix() const override;
};

#endif
