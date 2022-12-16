#ifndef MAP_FACE_H
#define MAP_FACE_H

#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/drawable/Drawable.hpp"

class MapFace : public Drawable
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
    MapFace(Graphics& gfx, Mesh const& mesh);
    ~MapFace() override;
    void ChangeTexture(Graphics& gfx, char const* filename);
    void Update(Graphics& gfx) override;
};

#endif
