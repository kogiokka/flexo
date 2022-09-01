#ifndef LATTICE_VERTEX_H
#define LATTICE_VERTEX_H

#include <glm/glm.hpp>

#include "Graphics.hpp"
#include "Mesh.hpp"
#include "bindable/Texture2D.hpp"
#include "drawable/InstancedDrawable.hpp"

class LatticeVertex : public InstancedDrawable
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
            float alpha;
        };
        Frag frag;
    };

    UniformBlock m_ub;

public:
    LatticeVertex(Graphics& gfx, Mesh const& instanceMesh, Mesh const& perInstanceData);
    ~LatticeVertex() override;
    void Update(Graphics& gfx) override;
    std::string GetName() const override;
};
#endif
