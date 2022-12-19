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
#include "gfx/drawable/Drawable.hpp"
#include "gfx/drawable/InstancedDrawable.hpp"

class VolumetricModel : public Drawable
{
    friend WatermarkingProject;

public:
    VolumetricModel(Graphics& gfx, Mesh mesh);
    ~VolumetricModel() override;
    void ChangeTexture(Graphics& gfx, char const* filename);
    void Update(Graphics& gfx) override;

private:
    UniformBlock m_ublight;
    UniformBlock m_ubmat;
    UniformBlock m_ubo;
};

#endif
