#ifndef VOLUMETRIC_MODEL_H
#define VOLUMETRIC_MODEL_H

#include <memory>

#include <glm/glm.hpp>

#include "Graphics.hpp"
#include "Mesh.hpp"
#include "bindable/Texture2D.hpp"
#include "drawable/InstancedDrawable.hpp"

class VolumetricModel : public InstancedDrawable
{
    struct UniformBlock {
        struct Vert {
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
            bool isWatermarked;
        };

        Vert vert;
    };

    UniformBlock m_ub;
    std::shared_ptr<Bind::Texture2D> m_texColor;
    std::shared_ptr<Bind::Texture2D> m_texPattern;

public:
    VolumetricModel(Graphics& gfx, Mesh const& instanceMesh, Mesh const& perInstanceData);
    ~VolumetricModel() override;
    void Update(Graphics& gfx) override;
    glm::mat4 GetTransformMatrix() const override;
    std::string GetName() const override;
};

#endif
