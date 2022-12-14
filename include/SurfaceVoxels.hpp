#ifndef VOXEL_SURFACE_H
#define VOXEL_SURFACE_H

#include <glm/glm.hpp>
#include <vector>

#include "Mesh.hpp"
#include "VolumetricModelData.hpp"
#include "Voxel.hpp"

class SurfaceVoxels
{
public:
    SurfaceVoxels(VolumetricModelData& modelData);
    std::vector<Voxel> const& Voxels();
    Mesh GenMesh();
    std::vector<glm::vec3> GenPositions();

private:
    glm::vec3 m_scale;
    std::vector<Voxel> m_voxels;
};

#endif
