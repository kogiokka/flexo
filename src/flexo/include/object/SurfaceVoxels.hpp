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

class SurfaceVoxels : public Object
{
public:
    SurfaceVoxels(VolumetricModelData const& modelData);
    virtual ~SurfaceVoxels() = default;
    std::vector<Voxel> const& Voxels() const;
    virtual std::vector<glm::vec3> GetPositions() const override;
    std::future<void> Parameterize(Map<3, 2> const& map, float& progress);
    void GenerateMesh();

private:
    void ApplyTransform() override;
    void GenEditableMesh();

    glm::vec3 m_scale;
    std::vector<Voxel> m_voxels;
};

#endif
