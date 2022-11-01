#ifndef VOLUMETRIC_MODEL_H
#define VOLUMETRIC_MODEL_H

#include <cstdint>
#include <memory>

#include <rvl.h>

#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "VolumetricModelData.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/bindable/Texture2D.hpp"
#include "gfx/drawable/InstancedDrawable.hpp"

class VolumetricModel : public Drawable
{
    friend WatermarkingProject;

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

public:
    VolumetricModel(Graphics& gfx, VolumetricModelData& model);
    ~VolumetricModel() override;
    void ChangeTexture(Graphics& gfx, char const* filename);
    void Update(Graphics& gfx) override;
    std::string GetName() const override;

private:
    Mesh m_mesh;

    void CreateMesh(VolumetricModelData& model);
    void AddFace(Mesh const& face, glm::vec3 offset, glm::vec3 scale);
};

#endif
