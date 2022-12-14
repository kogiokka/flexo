#ifndef VOXEL_SURFACE_H
#define VOXEL_SURFACE_H

#include <glm/glm.hpp>
#include <vector>

#include "Mesh.hpp"
#include "VolumetricModelData.hpp"

struct Voxel {
    Voxel(Mesh mesh, glm::vec2 texcrd)
        : mMesh(mesh)
        , mTexcrd(texcrd)
    {
    }
    Mesh mMesh;
    glm::vec2 mTexcrd;
};

class SurfaceVoxels
{
public:
    SurfaceVoxels(VolumetricModelData& modelData);
    Mesh GenMesh();

    std::vector<Voxel> m_voxels;
};

#endif
