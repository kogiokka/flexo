#ifndef VOXEL_SURFACE_H
#define VOXEL_SURFACE_H

#include <future>
#include <vector>

#include <glm/glm.hpp>

#include "Map.hpp"
#include "Mesh.hpp"
#include "Object.hpp"
#include "Voxel.hpp"

class VolumetricModelData;
class Graphics;

class SurfaceVoxels : public Object
{
public:
    SurfaceVoxels(VolumetricModelData& modelData);
    virtual ~SurfaceVoxels() = default;
    std::vector<Voxel> const& Voxels() const;
    virtual std::vector<glm::vec3> GetPositions() const override;
    std::future<void> Parameterize(Map<3, 2> const& map, float& progress);
    virtual void GenerateDrawables(Graphics& gfx) override;

private:
    void ApplyTransform() override;
    virtual Mesh GenerateMesh() const override;
    virtual Wireframe GenerateWireMesh() const override;

    glm::vec3 m_scale;
    std::vector<Voxel> m_voxels;
};

#endif
