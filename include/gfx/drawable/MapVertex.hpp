#ifndef MAP_VERTEX_H
#define MAP_VERTEX_H

#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/bindable/Texture2D.hpp"
#include "gfx/drawable/InstancedDrawable.hpp"

class MapVertex : public InstancedDrawable
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
    MapVertex(Graphics& gfx, Mesh const& instanceMesh, std::vector<glm::vec3> const& perInstanceData);
    ~MapVertex() override;
    void Update(Graphics& gfx) override;
};
#endif
