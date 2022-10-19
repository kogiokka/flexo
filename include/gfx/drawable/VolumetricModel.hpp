#ifndef VOLUMETRIC_MODEL_H
#define VOLUMETRIC_MODEL_H

#include <cstdint>
#include <memory>

#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/bindable/Texture2D.hpp"
#include "gfx/drawable/InstancedDrawable.hpp"

enum VoxelExposedDir : unsigned char {
    VoxelExposedDir_X_Pos = 1 << 1,
    VoxelExposedDir_Y_Pos = 1 << 2,
    VoxelExposedDir_Z_Pos = 1 << 3,
    VoxelExposedDir_X_Neg = 1 << 4,
    VoxelExposedDir_Y_Neg = 1 << 5,
    VoxelExposedDir_Z_Neg = 1 << 6,
};

class VolumetricModel : public Drawable
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

public:
    VolumetricModel(Graphics& gfx, glm::ivec3 resolution, uint8_t const* data);
    ~VolumetricModel() override;
    void ChangeTexture(Graphics& gfx, char const* filename);
    void Update(Graphics& gfx) override;
    std::string GetName() const override;

private:
    Mesh CreateMesh();

    glm::ivec3 m_resolution;
    glm::vec3 m_vxDims;
    uint8_t const* m_data;
};

#endif
