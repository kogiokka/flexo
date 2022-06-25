#ifndef LATTICE_VERTEX_H
#define LATTICE_VERTEX_H

#include <glm/glm.hpp>

#include "Graphics.hpp"
#include "Mesh.hpp"
#include "bindable/Texture2D.hpp"
#include "drawable/Drawable.hpp"

class LatticeVertex : public Drawable
{
    struct UniformBlock {
        struct Vert {
            glm::mat4 viewProjMat;
            glm::mat4 modelMat;
        };

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
            float alpha;
        };

        Vert vert;
        Frag frag;
    };

    GLuint m_count;
    UniformBlock m_ub;

public:
    LatticeVertex(Graphics& gfx, Mesh const& mesh);
    void Draw(Graphics& gfx) const override;
    void Update(Graphics& gfx) override;
};
#endif

